#include "Automata.h"
#include "Letra.h"
#include "ResourceManager.h"
#include <QDebug>
#include <QRandomGenerator>
#include <cmath>
#include <cfloat>

Automata::Automata(QObject *parent)
    : QObject(parent),
      Entidad(80.0f, 250.0f),
      automataActivo(false),
      velocidadMovimiento(250.0f),
      direccion(FRENTE),
      anchoMundo(1024.0f),
      altoMundo(768.0f),
      frameAnimacion(0),
      tiempoAnimacion(0.0f),
      estaQuieto(true),
      tiempoHastaProximaAccion(0.0f),
      letraObjetivo(nullptr),
      tieneMision(false),
      buscandoLetra(false),
      posicionXArriba(400.0f),
      estaBajando(false),
      posicionDesdeQueBaja(650.0f),
      direccionRodeo(0),           // NUEVO: Para recordar dirección de rodeo
      tiempoAtascado(0.0f),        // NUEVO: Contador de tiempo atascado
      ultimaPosX(0.0f),
      ultimaPosY(0.0f)
{
    cargarSprites();
    qDebug() << "[Automata] Creado en posición:" << posX << posY;
}

Automata::~Automata()
{
    qDebug() << "[Automata] Destruido";
}

void Automata::cargarSprites()
{
    auto &rm = ResourceManager::getInstance();
    spriteFrente = rm.getSprite("nivel1_jugador_frente");
    spriteEspalda = rm.getSprite("nivel1_jugador_espalda");
    spriteIzquierda = rm.getSprite("nivel1_jugador_izquierda");
    spriteDerecha = rm.getSprite("nivel1_jugador_derecha");
}

void Automata::setLimites(float ancho, float alto)
{
    anchoMundo = ancho;
    altoMundo = alto;
}

void Automata::actualizar(float deltaTime)
{
    if (!estaQuieto) {
        tiempoAnimacion += deltaTime;
        if (tiempoAnimacion >= 0.15f) {
            cambiarFrame();
            tiempoAnimacion = 0.0f;
        }
    }
}

void Automata::actualizar(float deltaTime, QVector<Letra*> &letras)
{
    const int ANCHO_COL = 28;
    const int ALTO_COL = 44;
    const float VELOCIDAD = 3.5f;

    // =========================================================================
    // PRIORIDAD 1: Si tiene letra o está bajando -> secuencia subida/bajada
    // =========================================================================
    if (!letrasRecogidas.isEmpty() || estaBajando) {
        subirLetraArriba(deltaTime);
        return;
    }

    // =========================================================================
    // PRIORIDAD 2: Si NO está activo -> quedarse quieto
    // =========================================================================
    if (!automataActivo) {
        estaQuieto = true;
        return;
    }

    // =========================================================================
    // PRIORIDAD 3: Buscar y moverse hacia la letra objetivo
    // =========================================================================
    Letra* objetivo = encontrarLetraCercana(letras);
    
    if (!objetivo) {
        estaQuieto = true;
        return;
    }

    float targetX = objetivo->getPosX();
    float targetY = objetivo->getPosY();
    float distX = targetX - posX;
    float distY = targetY - posY;
    float distancia = std::sqrt(distX * distX + distY * distY);

    // =========================================================================
    // Si está en rango -> RECOGER LA LETRA
    // =========================================================================
    if (distancia <= RANGO_RECOGIDA) {
        qDebug() << "[Automata] ¡Recogiendo letra:" << objetivo->getLetra() << "!";
        objetivo->setRecogida(true);
        agregarLetra(objetivo);
        direccionRodeo = 0;
        tiempoAtascado = 0.0f;
        return;
    }

    // =========================================================================
    // MOVIMIENTO CON PATHFINDING MEJORADO
    // =========================================================================
    estaQuieto = false;
    
    float nuevaX = posX;
    float nuevaY = posY;
    bool seMovio = false;

    // Detectar si está atascado (no se ha movido significativamente)
    float movimientoDesdeUltimo = std::sqrt(
        (posX - ultimaPosX) * (posX - ultimaPosX) + 
        (posY - ultimaPosY) * (posY - ultimaPosY)
    );
    
    if (movimientoDesdeUltimo < 1.0f) {
        tiempoAtascado += deltaTime;
    } else {
        tiempoAtascado = 0.0f;
        direccionRodeo = 0;  // Resetear dirección de rodeo si se movió bien
    }
    
    ultimaPosX = posX;
    ultimaPosY = posY;

    // Si está muy atascado, cambiar estrategia de rodeo
    if (tiempoAtascado > 0.5f) {
        direccionRodeo = (direccionRodeo == 0) ? 
            ((QRandomGenerator::global()->bounded(2) == 0) ? 1 : -1) : 
            -direccionRodeo;  // Cambiar dirección
        tiempoAtascado = 0.0f;
    }

    // =========================================================================
    // INTENTAR MOVIMIENTO DIRECTO PRIMERO
    // =========================================================================
    
    // Normalizar dirección
    float norma = std::sqrt(distX * distX + distY * distY);
    float dirX = (norma > 0) ? distX / norma : 0;
    float dirY = (norma > 0) ? distY / norma : 0;

    // Intentar moverse directamente hacia el objetivo
    nuevaX = posX + dirX * VELOCIDAD;
    nuevaY = posY + dirY * VELOCIDAD;

    if (!hayColision(nuevaX, nuevaY, ANCHO_COL, ALTO_COL) && dentroLimites(nuevaX, nuevaY)) {
        posX = nuevaX;
        posY = nuevaY;
        seMovio = true;
        actualizarDireccionSprite(dirX, dirY);
    } else {
        // =====================================================================
        // RODEO DE OBSTÁCULOS
        // =====================================================================
        
        // Decidir dirección de rodeo (1 = derecha/abajo, -1 = izquierda/arriba)
        if (direccionRodeo == 0) {
            direccionRodeo = (QRandomGenerator::global()->bounded(2) == 0) ? 1 : -1;
        }

        // Si el obstáculo está más en X, rodear por Y (y viceversa)
        bool obstaculoEnX = (std::abs(distX) > std::abs(distY));
        
        QVector<std::pair<float, float>> intentos;
        
        if (obstaculoEnX) {
            // Obstáculo horizontal -> intentar rodear verticalmente
            intentos.append({posX, posY + VELOCIDAD * direccionRodeo});
            intentos.append({posX, posY - VELOCIDAD * direccionRodeo});
            intentos.append({posX + VELOCIDAD * direccionRodeo, posY + VELOCIDAD * direccionRodeo});
            intentos.append({posX - VELOCIDAD * direccionRodeo, posY + VELOCIDAD * direccionRodeo});
        } else {
            // Obstáculo vertical -> intentar rodear horizontalmente
            intentos.append({posX + VELOCIDAD * direccionRodeo, posY});
            intentos.append({posX - VELOCIDAD * direccionRodeo, posY});
            intentos.append({posX + VELOCIDAD * direccionRodeo, posY + VELOCIDAD * direccionRodeo});
            intentos.append({posX + VELOCIDAD * direccionRodeo, posY - VELOCIDAD * direccionRodeo});
        }
        
        // Agregar más opciones de movimiento
        intentos.append({posX + VELOCIDAD, posY});
        intentos.append({posX - VELOCIDAD, posY});
        intentos.append({posX, posY + VELOCIDAD});
        intentos.append({posX, posY - VELOCIDAD});
        intentos.append({posX + VELOCIDAD, posY + VELOCIDAD});
        intentos.append({posX - VELOCIDAD, posY - VELOCIDAD});
        intentos.append({posX + VELOCIDAD, posY - VELOCIDAD});
        intentos.append({posX - VELOCIDAD, posY + VELOCIDAD});

        // Probar cada intento
        for (const auto& intento : intentos) {
            if (!hayColision(intento.first, intento.second, ANCHO_COL, ALTO_COL) && 
                dentroLimites(intento.first, intento.second)) {
                posX = intento.first;
                posY = intento.second;
                seMovio = true;
                actualizarDireccionSprite(intento.first - ultimaPosX, intento.second - ultimaPosY);
                break;
            }
        }
    }

    // Animación
    if (seMovio) {
        tiempoAnimacion += deltaTime;
        if (tiempoAnimacion >= 0.15f) {
            cambiarFrame();
            tiempoAnimacion = 0.0f;
        }
    }
}

void Automata::actualizarDireccionSprite(float dx, float dy)
{
    if (std::abs(dx) > std::abs(dy)) {
        direccion = (dx > 0) ? DERECHA : IZQUIERDA;
    } else {
        direccion = (dy > 0) ? FRENTE : ESPALDA;
    }
}

bool Automata::dentroLimites(float x, float y)
{
    const float MARGEN = 70.0f;
    return (x >= MARGEN && 
            x <= anchoMundo - MARGEN && 
            y >= 220.0f && 
            y <= altoMundo - 50.0f);
}

void Automata::interactuarConJugador(char letraFaltante)
{
    qDebug() << "[Automata] ¡Activado! Buscando letra:" << letraFaltante;
    automataActivo = true;
    letraABuscar = letraFaltante;
    tieneMision = true;
    direccionRodeo = 0;
    tiempoAtascado = 0.0f;
}

void Automata::subirLetraArriba(float deltaTime)
{
    const float TARGET_Y_ARRIBA = 250.0f;
    const float TARGET_Y_ABAJO = 600.0f;
    const float VELOCIDAD_VERTICAL = 100.0f;

    if (!estaBajando) {
        // SUBIENDO
        if (posY > TARGET_Y_ARRIBA) {
            posY -= VELOCIDAD_VERTICAL * deltaTime;
            direccion = ESPALDA;
            
            tiempoAnimacion += deltaTime;
            if (tiempoAnimacion >= 0.15f) {
                cambiarFrame();
                tiempoAnimacion = 0.0f;
            }
        } else {
            // Llegó arriba -> SOLTAR LETRA
            posY = TARGET_Y_ARRIBA;
            
            if (!letrasRecogidas.isEmpty()) {
                Letra* letra = letrasRecogidas[0];
                
                // Soltar con movimiento parabólico
                float soltarX = posX + (QRandomGenerator::global()->bounded(60) - 30);
                float soltarY = posY;
                
                soltarX = qBound(100.0f, soltarX, 920.0f);
                
                letra->setSoltada(soltarX, soltarY);
                letrasRecogidas.removeAll(letra);
                
                qDebug() << "[Automata] Letra soltada:" << letra->getLetra();
                
                automataActivo = false;
                tieneMision = false;
                estaBajando = true;
            }
        }
    } else {
        // BAJANDO
        if (posY < TARGET_Y_ABAJO) {
            posY += VELOCIDAD_VERTICAL * deltaTime;
            direccion = FRENTE;
            
            tiempoAnimacion += deltaTime;
            if (tiempoAnimacion >= 0.15f) {
                cambiarFrame();
                tiempoAnimacion = 0.0f;
            }
        } else {
            posY = TARGET_Y_ABAJO;
            estaBajando = false;
            estaQuieto = true;
            direccionRodeo = 0;
        }
    }
}

Letra* Automata::encontrarLetraCercana(QVector<Letra*> &letras)
{
    float menorDistancia = RANGO_VISION;
    Letra* letraCercana = nullptr;

    for (Letra* letra : letras) {
        if (letra->estaRecogida()) continue;
        
        // Si tiene misión específica, buscar solo esa letra
        if (tieneMision && letra->getLetra() != letraABuscar) {
            continue;
        }

        float distX = letra->getPosX() - posX;
        float distY = letra->getPosY() - posY;
        float distancia = std::sqrt(distX * distX + distY * distY);

        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            letraCercana = letra;
        }
    }

    return letraCercana;
}

void Automata::restriccionPantalla()
{
    if (posX < 70) posX = 70;
    if (posX + ANCHO_AUTOMATA > anchoMundo - 70) posX = anchoMundo - 70 - ANCHO_AUTOMATA;
    if (posY < 220) posY = 220;
    if (posY + ALTO_AUTOMATA > altoMundo - 50) posY = altoMundo - 50 - ALTO_AUTOMATA;
}

QPixmap Automata::getSpriteActual() const
{
    switch (direccion) {
    case FRENTE: return spriteFrente;
    case ESPALDA: return spriteEspalda;
    case IZQUIERDA: return spriteIzquierda;
    case DERECHA: return spriteDerecha;
    default: return spriteFrente;
    }
}

void Automata::cambiarFrame()
{
    frameAnimacion = (frameAnimacion == 0) ? 1 : 0;
}

void Automata::dibujar(QPainter &painter)
{
    QPixmap spriteDir = getSpriteActual();

    if (spriteDir.isNull()) {
        painter.fillRect(posX - 20, posY - 20, 40, 40, QColor(0, 100, 255, 150));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(posX - 15, posY + 5, "NPC");
        return;
    }

    const int numFrames = 2;
    int frameWidth = spriteDir.width() / numFrames;
    int frameHeight = spriteDir.height();
    QRect src(frameAnimacion * frameWidth, 0, frameWidth, frameHeight);

    int drawWidth = frameWidth / 2;
    int drawHeight = frameHeight / 2;
    int offsetX = posX - frameWidth / 4;
    int offsetY = posY - drawHeight;

    if (frameAnimacion == 1) {
        offsetX += 55;
    }

    QRect dst(offsetX, offsetY, drawWidth, drawHeight);
    painter.drawPixmap(dst, spriteDir, src);

    // Indicador si tiene letra
    if (!letrasRecogidas.isEmpty()) {
        painter.setPen(QPen(Qt::cyan, 2));
        painter.drawRect(dst);
    }
    
    // DEBUG: Mostrar si está activo
    if (automataActivo) {
        painter.setPen(Qt::green);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(posX - 20, posY - 50, "ACTIVO");
    }
}

bool Automata::hayColision(float x, float y, int ancho, int alto)
{
    QRect rect(x - ancho/2, y - alto/2, ancho, alto);

    for (const QRect &mesa : colisiones) {
        if (rect.intersects(mesa)) {
            return true;
        }
    }

    return false;
}

bool Automata::buscarRutaAlternativa(float &x, float &y, float targetX, float targetY,
                                     int ancho, int alto)
{
    // Este método ya no se usa directamente, pero lo mantengo por compatibilidad
    const float PASO = 3.0f;
    
    QVector<std::pair<float, float>> direcciones = {
        {PASO, 0}, {-PASO, 0}, {0, PASO}, {0, -PASO},
        {PASO, PASO}, {-PASO, -PASO}, {PASO, -PASO}, {-PASO, PASO}
    };

    for (const auto& dir : direcciones) {
        float nuevoX = x + dir.first;
        float nuevoY = y + dir.second;
        
        if (!hayColision(nuevoX, nuevoY, ancho, alto) && dentroLimites(nuevoX, nuevoY)) {
            x = nuevoX;
            y = nuevoY;
            return true;
        }
    }

    return false;
}
