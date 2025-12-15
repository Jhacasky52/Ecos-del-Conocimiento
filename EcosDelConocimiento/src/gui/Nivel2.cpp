#include "Nivel2.h"
#include "ResourceManager.h"
#include "DifficultyManager.h"
#include <QRandomGenerator>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Nivel2::Nivel2(QObject *parent)
    : QObject(parent),
      jugador(nullptr),
      frameActual(0),
      tiempoFrame(0.0f),
      seEstaMoviendo(false),
      offsetMundo(0.0f),
      velocidadY(0.0f),
      enSuelo(true),
      mirandoDerecha(true),
      tiempoSpawnFuego(3.0f),
      tiempoSpawnPergamino(2.0f),
      pergaminoRecogidoIndex(-1),
      jugadorTieneCubeta(false),
      cuervoX(800.0f),
      cuervoY(200.0f),
      cuervoVelX(0.0f),
      cuervoVelY(0.0f),
      cuervoMirandoDerecha(true),
      cuervoFrame(0),
      cuervoTiempoFrame(0.0f),
      estadoCuervo(CUERVO_VOLANDO),
      pergaminoObjetivoCuervo(-1),
      cuervoTiempoEspera(3.0f),
      pergaminosRobados(0),
      tiempoRestante(TIEMPO_LIMITE),
      juegoActivo(false),
      puntuacion(0),
      pergaminosEntregados(0)
{
    qDebug() << "\n========== [Nivel2] Biblioteca en Llamas - Inicializando ==========";

    cargarRecursos();
    
    // Crear jugador (posición inicial en el centro del mundo)
    jugador = new Jugador(this);
    jugador->setLimites(MUNDO_ANCHO, 768.0f);
    jugador->setPosicion(400.0f, SUELO_Y);  // Posición en el mundo
    jugador->setVelocidadMovimiento(VELOCIDAD_JUGADOR);

    // Posiciones de fuente y cofre en el mundo
    fuenteX = 1100.0f;     // Fuente a mitad de camino
    cofreX = 2200.0f;      // Cofre a la derecha (lejos)

    // Inicializar cubeta
    cubetaLanzada.activa = false;
    cubetaLanzada.enVuelo = false;

    iniciarJuego();
    
    qDebug() << "[Nivel2] Fuente en X:" << fuenteX;
    qDebug() << "[Nivel2] Cofre en X:" << cofreX;
    qDebug() << "[Nivel2] Mundo ancho:" << MUNDO_ANCHO;
    qDebug() << "========== [Nivel2] Inicializado ==========\n";
}

Nivel2::~Nivel2()
{
    if (jugador) delete jugador;
    qDebug() << "[Nivel2] Destruido";
}

void Nivel2::cargarRecursos()
{
    auto &rm = ResourceManager::getInstance();
    
    // Parallax
    fondoLejano = rm.getSprite("nivel2_fondo_lejano");
    fondoMedio = rm.getSprite("nivel2_fondo_medio");
    fondoCerca = rm.getSprite("nivel2_fondo_cerca");
    
    // Jugador
    spriteJugadorDerecha = rm.getSprite("nivel2_jugador_derecha");
    spriteJugadorIzquierda = rm.getSprite("nivel2_jugador_izquierda");
    spriteSaltoDerecha = rm.getSprite("nivel2_salto_derecha");
    spriteSaltoIzquierda = rm.getSprite("nivel2_salto_izquierda");
    
    // Objetos
    spriteFuegoAire = rm.getSprite("nivel2_fuego_aire");
    spriteFuegoPiso = rm.getSprite("nivel2_fuego_piso");
    spritePergamino = rm.getSprite("nivel2_pergamino");
    spritePergaminoPocoFuego = rm.getSprite("nivel2_pergamino_pocofuego");
    spritePergaminoMuchoFuego = rm.getSprite("nivel2_pergamino_muchofuego");
    spriteFuente = rm.getSprite("nivel2_fuente");
    spriteCofre = rm.getSprite("nivel2_cofre");
    spriteCubeta = rm.getSprite("nivel2_cubeta");
    
    // Cuervo
    spriteCuervoDerecha = rm.getSprite("nivel2_cuervo_derecha");
    spriteCuervoDerecha2 = rm.getSprite("nivel2_cuervo_derecha2");
    spriteCuervoIzquierda = rm.getSprite("nivel2_cuervo_izquierda");
    spriteCuervoIzquierda2 = rm.getSprite("nivel2_cuervo_izquierda2");
    
    qDebug() << "[Nivel2] Recursos cargados";
}

void Nivel2::iniciarJuego()
{
    // Usar tiempo límite según dificultad
    tiempoRestante = DifficultyManager::getInstance().getTiempoLimiteNivel2();
    
    juegoActivo = true;
    puntuacion = 0;
    pergaminosEntregados = 0;
    pergaminoRecogidoIndex = -1;
    jugadorTieneCubeta = false;
    
    fuegosAire.clear();
    fuegosPiso.clear();
    pergaminos.clear();
    
    // Ajustar tiempos de spawn según dificultad
    tiempoSpawnFuego = DifficultyManager::getInstance().getIntervaloSpawnFuego();
    tiempoSpawnPergamino = DifficultyManager::getInstance().getIntervaloSpawnPergamino();
    
    qDebug() << "[Nivel2] Juego iniciado - Entregar" << DifficultyManager::getInstance().getPergaminosParaGanar() << "pergaminos";
    qDebug() << "[Nivel2] Dificultad:" << DifficultyManager::getInstance().getNombreDificultad();
    qDebug() << "[Nivel2] Tiempo límite:" << tiempoRestante << "segundos";
}

// =========================================================================
// CONVERSIÓN MUNDO <-> PANTALLA
// =========================================================================

float Nivel2::mundoAPantalla(float xMundo)
{
    // Convertir posición del mundo a posición en pantalla
    return xMundo - offsetMundo;
}

float Nivel2::pantallaAMundo(float xPantalla)
{
    return xPantalla + offsetMundo;
}

// =========================================================================
// ACTUALIZACIÓN PRINCIPAL
// =========================================================================

void Nivel2::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    if (!juegoActivo) return;
    
    // Tiempo
    tiempoRestante -= deltaTime;
    if (tiempoRestante <= 0) {
        finalizarJuego(false);
        return;
    }
    
    // Movimiento del jugador
    actualizarJugador(izquierda, derecha, deltaTime);
    
    // Animación
    actualizarAnimacion(deltaTime);
    
    // Saltar con W o arriba
    if (arriba) {
        intentarSaltar();
    }
    
    // Actualizar elementos del juego
    actualizarFuegosAire(deltaTime);
    actualizarFuegosPiso(deltaTime);
    actualizarPergaminos(deltaTime);
    actualizarCubeta(deltaTime);
    actualizarCuervo(deltaTime);
    
    // Spawn de fuego - más frecuente en dificultades altas
    tiempoSpawnFuego -= deltaTime;
    if (tiempoSpawnFuego <= 0) {
        spawnFuego();
        tiempoSpawnFuego = DifficultyManager::getInstance().getIntervaloSpawnFuego() 
                          + QRandomGenerator::global()->bounded(20) / 10.0f;
    }
    
    // Spawn de pergaminos - más frecuente en dificultad fácil
    tiempoSpawnPergamino -= deltaTime;
    if (tiempoSpawnPergamino <= 0) {
        spawnPergamino();
        tiempoSpawnPergamino = DifficultyManager::getInstance().getIntervaloSpawnPergamino() 
                              + QRandomGenerator::global()->bounded(30) / 10.0f;
    }
    
    // Victoria
    if (pergaminosEntregados >= DifficultyManager::getInstance().getPergaminosParaGanar()) {
        finalizarJuego(true);
    }
}

void Nivel2::actualizarJugador(bool izquierda, bool derecha, float deltaTime)
{
    if (!jugador) return;

    float jugadorX = jugador->getPosX();
    float deltaX = 0;
    seEstaMoviendo = false;  // ← REINICIA A FALSE

    // Movimiento horizontal
    if (izquierda) {
        deltaX = -VELOCIDAD_JUGADOR * deltaTime;
        mirandoDerecha = false;
        seEstaMoviendo = true;  // ← ACTIVA SI SE PRESIONA
    }

    if (derecha) {
        deltaX = VELOCIDAD_JUGADOR * deltaTime;
        mirandoDerecha = true;
        seEstaMoviendo = true;  // ← ACTIVA SI SE PRESIONA
    }

    // Verificar si puede moverse (máximo 2 fuegos)
    if (deltaX != 0) {
        float nuevaX = jugadorX + deltaX;
        int fuegosEnCamino = contarFuegosEntre(jugadorX, nuevaX);

        if (fuegosEnCamino <= 2 || !enSuelo) {
            // Puede moverse
            nuevaX = qBound(50.0f, nuevaX, MUNDO_ANCHO - 50.0f);
            jugador->setPosX(nuevaX);

            // Actualizar offset del mundo (cámara sigue al jugador)
            float centroPatalla = 512.0f;
            offsetMundo = nuevaX - centroPatalla;
            offsetMundo = qBound(0.0f, offsetMundo, MUNDO_ANCHO - 1024.0f);
        }
    }

    // Gravedad
    if (!enSuelo) {
        velocidadY += GRAVEDAD * deltaTime;
    }

    // Movimiento vertical
    float nuevaY = jugador->getPosY() + velocidadY * deltaTime;

    // Verificar suelo
    if (nuevaY >= SUELO_Y) {
        nuevaY = SUELO_Y;
        velocidadY = 0;
        enSuelo = true;
    }

    jugador->setPosY(nuevaY);

    // Verificar daño por fuego (si está en el suelo)
    if (enSuelo && hayFuegoEnPosicion(jugador->getPosX())) {
        // Empujar al jugador fuera del fuego
        if (mirandoDerecha) {
            jugador->setPosX(jugador->getPosX() - 30);
        } else {
            jugador->setPosX(jugador->getPosX() + 30);
        }

        puntuacion -= 5;
    }
}

void Nivel2::actualizarAnimacion(float deltaTime)
{
    // Solo animar si se está moviendo
    if (seEstaMoviendo) {
        tiempoFrame += deltaTime;
        if (tiempoFrame >= TIEMPO_POR_FRAME) {
            tiempoFrame = 0;
            frameActual = (frameActual + 1) % 2;
        }
    } else {
        // Si no se mueve, reinicia a frame 0 (quieto)
        frameActual = 0;
        tiempoFrame = 0.0f;
    }
}

// =========================================================================
// FUEGO
// =========================================================================

void Nivel2::spawnFuego()
{
    FuegoAire f;
    // Spawn en área visible + un poco más
    f.x = pantallaAMundo(QRandomGenerator::global()->bounded(900) + 50.0f);
    f.y = -50.0f;
    f.velocidadY = VELOCIDAD_CAIDA_FUEGO;
    f.activo = true;
    fuegosAire.append(f);
}

void Nivel2::actualizarFuegosAire(float deltaTime)
{
    for (int i = fuegosAire.size() - 1; i >= 0; i--) {
        FuegoAire &f = fuegosAire[i];
        
        f.y += f.velocidadY * deltaTime;
        
        // Llegó al piso
        if (f.y >= SUELO_Y - FUEGO_TAMANIO/2) {
            esparcirFuegoEnPiso(f.x);
            fuegosAire.removeAt(i);
        }
    }
}

void Nivel2::esparcirFuegoEnPiso(float x)
{
    // Crear 3 fuegos adyacentes
    for (int i = -1; i <= 1; i++) {
        FuegoPiso fp;
        fp.x = x + i * FUEGO_TAMANIO;
        fp.tiempoVida = 0;
        fp.activo = true;
        fuegosPiso.append(fp);
    }
    qDebug() << "[Nivel2] Fuego esparcido en X:" << x;
}

void Nivel2::actualizarFuegosPiso(float deltaTime)
{
    for (int i = fuegosPiso.size() - 1; i >= 0; i--) {
        FuegoPiso &f = fuegosPiso[i];
        
        f.tiempoVida += deltaTime;
        
        // Desaparecer después de tiempo
        if (f.tiempoVida >= DifficultyManager::getInstance().getTiempoFuegoPiso()) {
            fuegosPiso.removeAt(i);
        }
    }
}

bool Nivel2::hayFuegoEnPosicion(float x)
{
    for (const FuegoPiso &f : fuegosPiso) {
        if (std::abs(f.x - x) < FUEGO_TAMANIO) {
            return true;
        }
    }
    return false;
}

int Nivel2::contarFuegosEntre(float x1, float x2)
{
    if (x1 > x2) std::swap(x1, x2);
    
    int count = 0;
    for (const FuegoPiso &f : fuegosPiso) {
        if (f.x >= x1 - FUEGO_TAMANIO/2 && f.x <= x2 + FUEGO_TAMANIO/2) {
            count++;
        }
    }
    return count;
}

void Nivel2::apagarFuegosEnArea(float x, float radio)
{
    for (int i = fuegosPiso.size() - 1; i >= 0; i--) {
        if (std::abs(fuegosPiso[i].x - x) < radio) {
            fuegosPiso.removeAt(i);
            puntuacion += 10;
        }
    }
    qDebug() << "[Nivel2] Fuegos apagados en X:" << x;
}

// =========================================================================
// PERGAMINOS
// =========================================================================

void Nivel2::spawnPergamino()
{
    Pergamino p;
    // Spawn en área visible pero NO cerca del cofre (cofre está en X=2200)
    // Limitar spawn entre X=100 y X=1800 (lejos del cofre)
    float minX = offsetMundo + 50.0f;
    float maxX = qMin(offsetMundo + 950.0f, 800.0f);  // No más allá de 1800
    
    if (maxX > minX) {
        p.x = minX + QRandomGenerator::global()->bounded(static_cast<int>(maxX - minX));
    } else {
        p.x = minX + 200.0f;
    }
    
    p.y = -50.0f;
    p.velocidadY = VELOCIDAD_CAIDA_PERGAMINO;
    p.enAire = true;
    p.recogido = false;
    p.estado = NORMAL;
    p.tiempoEnFuego = 0;
    p.activo = true;
    pergaminos.append(p);
}

void Nivel2::actualizarPergaminos(float deltaTime)
{
    for (int i = pergaminos.size() - 1; i >= 0; i--) {
        Pergamino &p = pergaminos[i];
        
        if (!p.activo) continue;
        
        // Si está recogido, sigue al jugador
        if (p.recogido && i == pergaminoRecogidoIndex) {
            p.x = jugador->getPosX();
            p.y = jugador->getPosY() - 80;
            continue;
        }
        
        // Caer
        if (p.enAire) {
            p.y += p.velocidadY * deltaTime;
            
            if (p.y >= SUELO_Y - PERGAMINO_TAMANIO/2) {
                p.y = SUELO_Y - PERGAMINO_TAMANIO/2;
                p.enAire = false;
            }
        }
        
        // Verificar contacto con fuego (solo si está en el suelo y no recogido)
        if (!p.enAire && !p.recogido && p.estado != QUEMADO) {
            if (hayFuegoEnPosicion(p.x)) {
                p.tiempoEnFuego += deltaTime;
                
                // Cambiar estado según tiempo
                if (p.tiempoEnFuego >= TIEMPO_POCO_FUEGO + TIEMPO_MUCHO_FUEGO) {
                    p.estado = QUEMADO;
                    p.activo = false;
                    puntuacion -= 30;
                    qDebug() << "[Nivel2] ¡Pergamino quemado!";
                } else if (p.tiempoEnFuego >= TIEMPO_POCO_FUEGO) {
                    p.estado = MUCHO_FUEGO;
                } else if (p.tiempoEnFuego > 0) {
                    p.estado = POCO_FUEGO;
                }
            }
        }
    }
}

void Nivel2::intentarRecogerPergamino()
{
    if (!jugador || pergaminoRecogidoIndex >= 0) return;  // Ya tiene uno
    // QUITADO: Ya no impide recoger si tiene cubeta
    // if (jugadorTieneCubeta) return;  // Tiene cubeta
    
    float jx = jugador->getPosX();
    float jy = jugador->getPosY();
    
    for (int i = 0; i < pergaminos.size(); i++) {
        Pergamino &p = pergaminos[i];
        
        if (!p.activo || p.recogido || p.estado == QUEMADO) continue;
        
        float dx = p.x - jx;
        float dy = p.y - jy;
        float dist = std::sqrt(dx*dx + dy*dy);
        
        if (dist < 80.0f) {
            p.recogido = true;
            pergaminoRecogidoIndex = i;
            qDebug() << "[Nivel2] Pergamino recogido (estado:" << p.estado << ")";
            return;
        }
    }
}

void Nivel2::soltarPergamino()
{
    if (pergaminoRecogidoIndex < 0) return;
    
    Pergamino &p = pergaminos[pergaminoRecogidoIndex];
    p.recogido = false;
    p.x = jugador->getPosX();
    p.y = SUELO_Y - PERGAMINO_TAMANIO/2;
    pergaminoRecogidoIndex = -1;
    
    qDebug() << "[Nivel2] Pergamino soltado";
}

void Nivel2::entregarPergamino()
{
    if (pergaminoRecogidoIndex < 0) return;
    
    float jx = jugador->getPosX();
    
    // Verificar si está cerca del cofre
    if (std::abs(jx - cofreX) < COFRE_ANCHO) {
        Pergamino &p = pergaminos[pergaminoRecogidoIndex];
        
        // Solo aceptar pergaminos no muy quemados
        if (p.estado != QUEMADO && p.estado != MUCHO_FUEGO) {
            p.activo = false;
            pergaminoRecogidoIndex = -1;
            pergaminosEntregados++;
            
            int bonus = (p.estado == NORMAL) ? 100 : 50;  // Menos puntos si estaba prendido
            puntuacion += bonus;
            
            qDebug() << "[Nivel2] ¡Pergamino entregado!" << pergaminosEntregados << "/" << DifficultyManager::getInstance().getPergaminosParaGanar();
        } else {
            qDebug() << "[Nivel2] Pergamino muy quemado, no se acepta";
            puntuacion -= 10;
        }
    } else {
        qDebug() << "[Nivel2] No estás cerca del cofre";
    }
}

// =========================================================================
// CUBETA DE AGUA
// =========================================================================

void Nivel2::intentarTomarAgua()
{
    if (jugadorTieneCubeta) return;
    // QUITADO: Ya no impide tomar agua si tiene pergamino
    // if (pergaminoRecogidoIndex >= 0) return;  // Tiene pergamino
    
    float jx = jugador->getPosX();
    
    // Verificar si está cerca de la fuente
    if (std::abs(jx - fuenteX) < FUENTE_ANCHO) {
        jugadorTieneCubeta = true;
        qDebug() << "[Nivel2] Cubeta de agua tomada";
    }
}

void Nivel2::lanzarCubeta()
{
    if (!jugadorTieneCubeta) return;
    
    cubetaLanzada.x = jugador->getPosX();
    cubetaLanzada.y = jugador->getPosY() - 40;
    
    // Ángulo más pronunciado para un arco más alto (60 grados en vez de 45)
    float angulo = 60.0f * M_PI / 180.0f;  // Cambiado de CUBETA_ANGULO (45) a 60 grados
    float direccion = mirandoDerecha ? 1.0f : -1.0f;
    
    // Velocidad inicial más alta para compensar el ángulo mayor
    float velInicial = CUBETA_VEL_INICIAL * 1.2f;  // 20% más de velocidad
    
    cubetaLanzada.velocidadX = velInicial * std::cos(angulo) * direccion;
    cubetaLanzada.velocidadY = -velInicial * std::sin(angulo);  // Más hacia arriba
    
    cubetaLanzada.activa = true;
    cubetaLanzada.enVuelo = true;
    jugadorTieneCubeta = false;
    
    qDebug() << "[Nivel2] ¡Cubeta lanzada con arco alto! VelY:" << cubetaLanzada.velocidadY;
}

void Nivel2::actualizarCubeta(float deltaTime)
{
    if (!cubetaLanzada.activa || !cubetaLanzada.enVuelo) return;
    
    // Física parabólica
    cubetaLanzada.velocidadY += GRAVEDAD * deltaTime;
    cubetaLanzada.x += cubetaLanzada.velocidadX * deltaTime;
    cubetaLanzada.y += cubetaLanzada.velocidadY * deltaTime;
    
    // Llegó al suelo
    if (cubetaLanzada.y >= SUELO_Y) {
        // Apagar fuegos en el área de impacto
        apagarFuegosEnArea(cubetaLanzada.x, CUBETA_RADIO_EFECTO);
        
        // También salvar pergaminos cercanos que estén prendiéndose
        for (Pergamino &p : pergaminos) {
            if (p.activo && !p.recogido && (p.estado == POCO_FUEGO || p.estado == MUCHO_FUEGO)) {
                if (std::abs(p.x - cubetaLanzada.x) < CUBETA_RADIO_EFECTO) {
                    p.estado = NORMAL;
                    p.tiempoEnFuego = 0;
                    qDebug() << "[Nivel2] Pergamino salvado del fuego!";
                }
            }
        }
        
        cubetaLanzada.activa = false;
        cubetaLanzada.enVuelo = false;
    }
}

// =========================================================================
// CUERVO AUTÓMATA (roba pergaminos)
// =========================================================================

void Nivel2::actualizarCuervo(float deltaTime)
{
    // Animación del cuervo
    cuervoTiempoFrame += deltaTime;
    if (cuervoTiempoFrame >= 0.2f) {
        cuervoTiempoFrame = 0;
        cuervoFrame = (cuervoFrame + 1) % 2;
    }
    
    switch (estadoCuervo) {
        case CUERVO_VOLANDO: {
            // Volar de un lado a otro buscando pergaminos
            cuervoTiempoEspera -= deltaTime;
            
            if (cuervoMirandoDerecha) {
                cuervoX += DifficultyManager::getInstance().getVelocidadCuervo() * deltaTime;
                if (cuervoX > offsetMundo + 900) cuervoMirandoDerecha = false;
            } else {
                cuervoX -= DifficultyManager::getInstance().getVelocidadCuervo() * deltaTime;
                if (cuervoX < offsetMundo + 100) cuervoMirandoDerecha = true;
            }
            
            // Buscar un pergamino para robar cada cierto tiempo
            if (cuervoTiempoEspera <= 0) {
                // Buscar pergamino disponible en el suelo
                for (int i = 0; i < pergaminos.size(); i++) {
                    Pergamino &p = pergaminos[i];
                    if (p.activo && !p.recogido && !p.enAire && 
                        p.estado != QUEMADO && i != pergaminoRecogidoIndex) {
                        // Encontró un objetivo
                        pergaminoObjetivoCuervo = i;
                        estadoCuervo = CUERVO_BAJANDO;
                        qDebug() << "[Nivel2] Cuervo: Bajando a robar pergamino" << i;
                        break;
                    }
                }
                cuervoTiempoEspera = 5.0f + QRandomGenerator::global()->bounded(50) / 10.0f;
            }
            break;
        }
        
        case CUERVO_BAJANDO: {
            // Bajar hacia el pergamino objetivo
            if (pergaminoObjetivoCuervo >= 0 && pergaminoObjetivoCuervo < pergaminos.size()) {
                Pergamino &p = pergaminos[pergaminoObjetivoCuervo];
                
                // Si el pergamino ya no está disponible, volver a volar
                if (!p.activo || p.recogido || p.estado == QUEMADO) {
                    estadoCuervo = CUERVO_VOLANDO;
                    pergaminoObjetivoCuervo = -1;
                    cuervoTiempoEspera = 2.0f;
                    break;
                }
                
                // Moverse hacia el pergamino
                float dx = p.x - cuervoX;
                float dy = p.y - 30 - cuervoY;  // Un poco arriba del pergamino
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist > 20) {
                    cuervoX += (dx / dist) * DifficultyManager::getInstance().getVelocidadCuervo() * 1.4f * deltaTime;
                    cuervoY += (dy / dist) * DifficultyManager::getInstance().getVelocidadCuervo() * 1.4f * deltaTime;
                    cuervoMirandoDerecha = dx > 0;
                } else {
                    // Llegó al pergamino
                    estadoCuervo = CUERVO_ROBANDO;
                    cuervoTiempoEspera = 0.5f;  // Tiempo para agarrar
                }
            } else {
                estadoCuervo = CUERVO_VOLANDO;
            }
            break;
        }
        
        case CUERVO_ROBANDO: {
            cuervoTiempoEspera -= deltaTime;
            
            if (cuervoTiempoEspera <= 0) {
                // Robar el pergamino
                if (pergaminoObjetivoCuervo >= 0 && pergaminoObjetivoCuervo < pergaminos.size()) {
                    Pergamino &p = pergaminos[pergaminoObjetivoCuervo];
                    if (p.activo && !p.recogido) {
                        p.activo = false;  // El cuervo se lo llevó
                        pergaminosRobados++;
                        puntuacion -= 50;
                        qDebug() << "[Nivel2] ¡Cuervo robó un pergamino! Total robados:" << pergaminosRobados;
                    }
                }
                estadoCuervo = CUERVO_ESCAPANDO;
            }
            break;
        }
        
        case CUERVO_ESCAPANDO: {
            // Escapar hacia arriba y fuera de la pantalla
            cuervoY -= DifficultyManager::getInstance().getVelocidadCuervo() * 1.4f * deltaTime;
            cuervoX += (cuervoMirandoDerecha ? 1 : -1) * DifficultyManager::getInstance().getVelocidadCuervo() * deltaTime;
            
            if (cuervoY < -50) {
                // Reaparecer del otro lado
                cuervoY = 80.0f;
                cuervoX = offsetMundo + (cuervoMirandoDerecha ? -100.0f : 1100.0f);
                cuervoMirandoDerecha = !cuervoMirandoDerecha;
                estadoCuervo = CUERVO_VOLANDO;
                pergaminoObjetivoCuervo = -1;
                cuervoTiempoEspera = 4.0f;
            }
            break;
        }
    }
}

// =========================================================================
// SALTO
// =========================================================================

void Nivel2::intentarSaltar()
{
    if (enSuelo) {
        velocidadY = FUERZA_SALTO;
        enSuelo = false;
        qDebug() << "[Nivel2] ¡Salto!";
    }
}

// =========================================================================
// DIBUJO
// =========================================================================

void Nivel2::dibujarFondoParallax(QPainter &painter)
{
    int ancho = 1024;
    int alto = 768;
    
    // Capa lejana (más lenta)
    if (!fondoLejano.isNull()) {
        QPixmap escalado = fondoLejano.scaled(ancho, alto, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int offsetX = -static_cast<int>(offsetMundo * VEL_PARALLAX_LEJANO) % ancho;
        painter.drawPixmap(offsetX, 0, escalado);
        painter.drawPixmap(offsetX + ancho, 0, escalado);
        if (offsetX > 0) painter.drawPixmap(offsetX - ancho, 0, escalado);
    } else {
        QLinearGradient grad(0, 0, 0, alto);
        grad.setColorAt(0, QColor(20, 10, 5));
        grad.setColorAt(1, QColor(60, 30, 15));
        painter.fillRect(0, 0, ancho, alto, grad);
    }
    
    // Capa media
    if (!fondoMedio.isNull()) {
        QPixmap escalado = fondoMedio.scaled(ancho, alto, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int offsetX = -static_cast<int>(offsetMundo * VEL_PARALLAX_MEDIO) % ancho;
        painter.drawPixmap(offsetX, 0, escalado);
        painter.drawPixmap(offsetX + ancho, 0, escalado);
        if (offsetX > 0) painter.drawPixmap(offsetX - ancho, 0, escalado);
    }
    
    // Capa cercana (referencia 1:1)
    if (!fondoCerca.isNull()) {
        QPixmap escalado = fondoCerca.scaled(ancho, alto, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int offsetX = -static_cast<int>(offsetMundo * VEL_PARALLAX_CERCA) % ancho;
        painter.drawPixmap(offsetX, 0, escalado);
        painter.drawPixmap(offsetX + ancho, 0, escalado);
        if (offsetX > 0) painter.drawPixmap(offsetX - ancho, 0, escalado);
    }
}

void Nivel2::dibujarFuente(QPainter &painter)
{
    float screenX = mundoAPantalla(fuenteX);
    
    if (screenX > -250 && screenX < 1280) {
        // Tamaño doble: 240x280
        if (!spriteFuente.isNull()) {
            QPixmap escalado = spriteFuente.scaled(180, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(static_cast<int>(screenX - 120), 
                              static_cast<int>(SUELO_Y - 200), escalado);
        } else {
            painter.setBrush(QColor(100, 100, 150));
            painter.setPen(QPen(QColor(70, 70, 120), 3));
            painter.drawRect(static_cast<int>(screenX - 100), 
                            static_cast<int>(SUELO_Y - 200), 200, 200);
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 16, QFont::Bold));
            painter.drawText(static_cast<int>(screenX - 50), 
                            static_cast<int>(SUELO_Y - 100), "FUENTE");
        }
        
        // Indicador "F para tomar agua"
        float jx = jugador ? jugador->getPosX() : 0;
        if (std::abs(jx - fuenteX) < FUENTE_ANCHO + 80 && !jugadorTieneCubeta) {
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.setPen(Qt::cyan);
            painter.drawText(static_cast<int>(screenX - 50), 
                            static_cast<int>(SUELO_Y - 280), "Presiona F");
        }
    }
}

void Nivel2::dibujarCofre(QPainter &painter)
{
    float screenX = mundoAPantalla(cofreX);
    
    if (screenX > -250 && screenX < 1280) {
        // Tamaño doble: 240x200
        if (!spriteCofre.isNull()) {
            QPixmap escalado = spriteCofre.scaled(200, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(static_cast<int>(screenX - 120), 
                              static_cast<int>(SUELO_Y - 170), escalado);
        } else {
            painter.setBrush(QColor(139, 90, 43));
            painter.setPen(QPen(QColor(100, 60, 20), 4));
            painter.drawRect(static_cast<int>(screenX - 120), 
                            static_cast<int>(SUELO_Y - 160), 240, 160);
            painter.setPen(Qt::yellow);
            painter.setFont(QFont("Arial", 18, QFont::Bold));
            painter.drawText(static_cast<int>(screenX - 40), 
                            static_cast<int>(SUELO_Y - 80), "COFRE");
        }
        
        // Indicador de progreso
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.setPen(Qt::white);
        painter.drawText(static_cast<int>(screenX - 35), 
                        static_cast<int>(SUELO_Y - 200), 
                        QString("%1/%2").arg(pergaminosEntregados).arg(DifficultyManager::getInstance().getPergaminosParaGanar()));
        
        // Indicador "E para entregar"
        float jx = jugador ? jugador->getPosX() : 0;
        if (std::abs(jx - cofreX) < COFRE_ANCHO + 80 && pergaminoRecogidoIndex >= 0) {
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.setPen(Qt::green);
            painter.drawText(static_cast<int>(screenX - 50), 
                            static_cast<int>(SUELO_Y - 220), "Presiona E");
        }
    }
}

void Nivel2::dibujarFuegosPiso(QPainter &painter)
{
    for (const FuegoPiso &f : fuegosPiso) {
        float screenX = mundoAPantalla(f.x);
        
        if (screenX > -150 && screenX < 1180) {
            // Tamaño doble: 100x100
            int fuegoSize = static_cast<int>(FUEGO_TAMANIO);
            
            if (!spriteFuegoPiso.isNull()) {
                QPixmap escalado = spriteFuegoPiso.scaled(fuegoSize, fuegoSize,
                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(static_cast<int>(screenX - fuegoSize/2),
                                  static_cast<int>(SUELO_Y - fuegoSize - 5),
                                  escalado);
            } else {
                painter.setBrush(QColor(255, 100, 0, 200));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPointF(screenX, SUELO_Y - fuegoSize/2 + 15), 
                                   fuegoSize/2, fuegoSize/2);
            }
        }
    }
}

void Nivel2::dibujarFuegosAire(QPainter &painter)
{
    for (const FuegoAire &f : fuegosAire) {
        float screenX = mundoAPantalla(f.x);
        
        if (screenX > -150 && screenX < 1180) {
            // Tamaño doble: 80x80
            if (!spriteFuegoAire.isNull()) {
                QPixmap escalado = spriteFuegoAire.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(static_cast<int>(screenX - 40),
                                  static_cast<int>(f.y - 40),
                                  escalado);
            } else {
                painter.setBrush(QColor(255, 200, 0, 200));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPointF(screenX, f.y), 40, 40);
            }
        }
    }
}

void Nivel2::dibujarPergaminos(QPainter &painter)
{
    for (int i = 0; i < pergaminos.size(); i++) {
        const Pergamino &p = pergaminos[i];
        
        if (!p.activo || (p.recogido && i == pergaminoRecogidoIndex)) continue;
        
        float screenX = mundoAPantalla(p.x);
        
        if (screenX > -100 && screenX < 1124) {
            QPixmap sprite;
            
            switch (p.estado) {
                case POCO_FUEGO:
                    sprite = spritePergaminoPocoFuego;
                    break;
                case MUCHO_FUEGO:
                    sprite = spritePergaminoMuchoFuego;
                    break;
                default:
                    sprite = spritePergamino;
                    break;
            }
            
            // Tamaño grande del pergamino (70x70)
            int pergaminoSize = static_cast<int>(PERGAMINO_TAMANIO);
            
            if (!sprite.isNull()) {
                QPixmap escalado = sprite.scaled(pergaminoSize, pergaminoSize,
                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(static_cast<int>(screenX - pergaminoSize/2),
                                  static_cast<int>(p.y - pergaminoSize/2),
                                  escalado);
            } else {
                QColor color = (p.estado == NORMAL) ? QColor(210, 180, 140) :
                              (p.estado == POCO_FUEGO) ? QColor(255, 200, 100) :
                              QColor(255, 100, 50);
                painter.setBrush(color);
                painter.setPen(QPen(Qt::darkRed, 2));
                painter.drawRect(static_cast<int>(screenX - pergaminoSize/2),
                                static_cast<int>(p.y - pergaminoSize/2), 
                                pergaminoSize, pergaminoSize);
            }
        }
    }
}

void Nivel2::dibujarCubeta(QPainter &painter)
{
    if (!cubetaLanzada.activa) return;
    
    float screenX = mundoAPantalla(cubetaLanzada.x);
    
    // Tamaño doble: 70x70
    if (!spriteCubeta.isNull()) {
        QPixmap escalado = spriteCubeta.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(screenX - 35),
                          static_cast<int>(cubetaLanzada.y - 35),
                          escalado);
    } else {
        painter.setBrush(QColor(100, 150, 255));
        painter.setPen(QPen(Qt::blue, 2));
        painter.drawEllipse(QPointF(screenX, cubetaLanzada.y), 30, 30);
    }
}

void Nivel2::dibujarJugador(QPainter &painter)
{
    if (!jugador) return;

    float screenX = mundoAPantalla(jugador->getPosX());
    float jy = jugador->getPosY();
    QPixmap spriteActual;

    // Seleccionar sprite según estado
    if (!enSuelo) {
        spriteActual = mirandoDerecha ? spriteSaltoDerecha : spriteSaltoIzquierda;
    } else {
        spriteActual = mirandoDerecha ? spriteJugadorDerecha : spriteJugadorIzquierda;
    }

    if (!spriteActual.isNull()) {
        // El sprite sheet tiene 2 frames lado a lado (612x408 = 2 frames de 306x408)
        int spriteSheetWidth = spriteActual.width();
        int spriteSheetHeight = spriteActual.height();
        int frameWidth = spriteSheetWidth / 2; // 306
        int frameHeight = spriteSheetHeight; // 408

        // Seleccionar frame según animación
        int srcX = frameActual * frameWidth;
        QRect srcRect(srcX, 0, frameWidth, frameHeight);

        // Extraer el frame
        QPixmap frame = spriteActual.copy(srcRect);

        // Tamaño destino (doble del original: 140x180)
        int destW = 100;
        int destH = 140;

        QPixmap escalado = frame.scaled(destW, destH, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // ⭐ OFFSET POR DIRECCIÓN Y FRAME
        int offsetX = static_cast<int>(screenX - destW/2);
        int offsetY = static_cast<int>(jy - destH + 10);

        // Ajuste SOLO para DERECHA (frame 1)
        if (mirandoDerecha && frameActual == 1) {
            offsetX += 20;  // Ajusta si sigue desalineado: -20, -10, 0, 10, 15, 20, etc.
        }
        // Ajuste SEPARADO para IZQUIERDA (frame 1)
        else if (!mirandoDerecha && frameActual == 1) {
            offsetX += 20;  // Ajusta si sigue desalineado: -30, -20, -15, -10, 0, etc.
        }

        painter.drawPixmap(offsetX, offsetY, escalado);

    } else {
        // Fallback
        painter.setBrush(QColor(100, 60, 40));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(static_cast<int>(screenX - 35),
                         static_cast<int>(jy - 120), 70, 120);
    }

    // Dibujar pergamino recogido encima del jugador (30 píxeles más cerca)
    if (pergaminoRecogidoIndex >= 0) {
        const Pergamino &p = pergaminos[pergaminoRecogidoIndex];
        QPixmap sprite = (p.estado == POCO_FUEGO) ? spritePergaminoPocoFuego : spritePergamino;
        if (!sprite.isNull()) {
            QPixmap escalado = sprite.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // 30 píxeles más cerca: -200 + 30 = -170
            painter.drawPixmap(static_cast<int>(screenX - 35),
                               static_cast<int>(jy - 170),
                               escalado);
        } else {
            painter.setBrush(QColor(210, 180, 140));
            painter.drawRect(static_cast<int>(screenX - 30),
                             static_cast<int>(jy - 160), 60, 70);
        }
    }

    // Dibujar cubeta si la tiene (30 píxeles más cerca)
    if (jugadorTieneCubeta) {
        if (!spriteCubeta.isNull()) {
            QPixmap escalado = spriteCubeta.scaled(90 , 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // 30 píxeles más cerca: -100 + 30 = -70
            painter.drawPixmap(static_cast<int>(screenX + 40),
                               static_cast<int>(jy - 70),
                               escalado);
        } else {
            painter.setBrush(QColor(100, 150, 255));
            painter.drawEllipse(static_cast<int>(screenX + 45),
                                static_cast<int>(jy - 60), 40, 40);
        }
    }
}

void Nivel2::dibujarCuervo(QPainter &painter)
{
    float screenX = mundoAPantalla(cuervoX);

    if (screenX > -200 && screenX < 1230) {
        QPixmap sprite;
        if (cuervoMirandoDerecha) {
            sprite = (cuervoFrame == 0) ? spriteCuervoDerecha : spriteCuervoDerecha2;
        } else {
            sprite = (cuervoFrame == 0) ? spriteCuervoIzquierda : spriteCuervoIzquierda2;
        }

        // Tamaño doble del cuervo: 200x160
        int cuervoAncho = 150;
        int cuervoAlto = 100;

        if (!sprite.isNull()) {
            QPixmap escalado = sprite.scaled(cuervoAncho, cuervoAlto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(static_cast<int>(screenX - cuervoAncho/2),
                              static_cast<int>(cuervoY - cuervoAlto/2),
                              escalado);
        } else {
            // Fallback
            painter.setBrush(QColor(30, 30, 30));
            painter.drawEllipse(QPointF(screenX, cuervoY), 80, 50);
        }

        // Indicador de estado
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QString estadoTexto;
        QColor colorEstado;
        switch (estadoCuervo) {
            case CUERVO_VOLANDO:
                estadoTexto = "Buscando...";
                colorEstado = Qt::yellow;
                break;
            case CUERVO_BAJANDO:
                estadoTexto = "¡BAJANDO!";
                colorEstado = QColor(255, 150, 0);
                break;
            case CUERVO_ROBANDO:
                estadoTexto = "¡ROBANDO!";
                colorEstado = Qt::red;
                break;
            case CUERVO_ESCAPANDO:
                estadoTexto = "¡JA JA JA!";
                colorEstado = Qt::magenta;
                break;
        }
        painter.setPen(colorEstado);
        painter.drawText(static_cast<int>(screenX - 40), static_cast<int>(cuervoY - 90), estadoTexto);
    }
}

void Nivel2::dibujarUI(QPainter &painter)
{
    // Panel superior
    painter.fillRect(0, 0, 1024, 70, QColor(0, 0, 0, 180));

    // Título
    painter.setFont(QFont("Georgia", 16, QFont::Bold));
    painter.setPen(QColor(255, 200, 100));
    painter.drawText(10, 28, "BIBLIOTECA EN LLAMAS");

    // Objetivo
    painter.setFont(QFont("Arial", 11));
    painter.setPen(Qt::white);
    painter.drawText(10, 50, QString("Pergaminos: %1 / %2").arg(pergaminosEntregados).arg(DifficultyManager::getInstance().getPergaminosParaGanar()));

    // Pergaminos robados
    painter.setPen(Qt::red);
    painter.drawText(180, 50, QString("Robados: %1").arg(pergaminosRobados));

    // Tiempo
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(tiempoRestante < 30 ? Qt::red : Qt::green);
    painter.drawText(850, 28, QString("Tiempo: %1s").arg(static_cast<int>(tiempoRestante)));

    // Puntuación
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(850, 50, QString("Pts: %1").arg(puntuacion));

    // Estado de items (ahora puede llevar ambos)
    painter.setPen(Qt::cyan);
    QString estado;
    if (jugadorTieneCubeta && pergaminoRecogidoIndex >= 0) {
        estado = "PERGAMINO + CUBETA";
    } else if (jugadorTieneCubeta) {
        estado = "CUBETA";
    } else if (pergaminoRecogidoIndex >= 0) {
        estado = "PERGAMINO";
    } else {
        estado = "LIBRE";
    }
    painter.drawText(450, 28, QString("Llevas: %1").arg(estado));

    // Fuegos activos
    painter.setPen(QColor(255, 100, 50));
    painter.drawText(450, 50, QString("Fuegos: %1").arg(fuegosPiso.size()));

    // Indicadores de dirección
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(150, 150, 150));

    float jx = jugador ? jugador->getPosX() : 0;
    if (jx > fuenteX + 200) {
        painter.drawText(20, 400, "← FUENTE");
    }
    if (jx < cofreX - 200) {
        painter.drawText(950, 400, "COFRE →");
    }

    // Controles
    painter.setFont(QFont("Arial", 9));
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(10, 758, "A/D: Mover | W: Saltar | R: Recoger | G: Soltar | F: Tomar agua | T: Lanzar | E: Entregar");

    // Mensaje de fin
    if (!juegoActivo) {
        painter.fillRect(0, 300, 1024, 150, QColor(0, 0, 0, 200));
        painter.setFont(QFont("Georgia", 32, QFont::Bold));

        if (pergaminosEntregados >= DifficultyManager::getInstance().getPergaminosParaGanar()) {
            painter.setPen(QColor(100, 255, 100));
            painter.drawText(280, 390, "¡BIBLIOTECA SALVADA!");
        } else {
            painter.setPen(Qt::red);
            painter.drawText(300, 390, "LA BIBLIOTECA ARDIÓ");
        }
    }
}

void Nivel2::dibujar(QPainter &painter)
{
    // 1. Fondo parallax
    dibujarFondoParallax(painter);

    // 2. Fuente y Cofre
    dibujarFuente(painter);
    dibujarCofre(painter);

    // 3. Fuegos del piso
    dibujarFuegosPiso(painter);

    // 4. Pergaminos
    dibujarPergaminos(painter);

    // 5. Fuegos del aire
    dibujarFuegosAire(painter);

    // 6. Cubeta
    dibujarCubeta(painter);

    // 7. Cuervo
    dibujarCuervo(painter);

    // 8. Jugador
    dibujarJugador(painter);

    // 9. UI
    dibujarUI(painter);
}

void Nivel2::finalizarJuego(bool victoria)
{
    juegoActivo = false;
    if (victoria) {
        emit nivelCompletado(puntuacion);
    } else {
        emit nivelFallido();
    }
}
