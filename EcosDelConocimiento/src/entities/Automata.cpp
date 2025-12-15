#include "Automata.h"
#include "Letra.h"
#include "ResourceManager.h"
#include <QDebug>
#include <QRandomGenerator>
#include <cmath>
#include <queue>


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
    indicePuntoRuta(0),
    tiempoRecalculoRuta(0.0f),
    ultimoObjetivo(-1, -1),
    tiempoAtascado(0.0f),
    ultimaPosX(0.0f),
    ultimaPosY(0.0f),
    intentosFallidos(0),
    letraEnMano(nullptr),
    offsetLetraX(0.0f),
    offsetLetraY(-30.0f)
{
    // Inicializar grid de navegación
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            gridNavegacion[x][y] = true;
        }
    }

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

// =============================================================================
// GENERACIÓN DEL GRID DE NAVEGACIÓN
// =============================================================================

void Automata::generarGridNavegacion()
{
    const int MARGEN_EXTRA = 15; //
    // Reiniciar grid - todo navegable por defecto
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            gridNavegacion[x][y] = true;
        }
    }

    // Marcar celdas ocupadas por obstáculos (con margen mejorado)
    for (const QRect &obstaculo : colisiones) {
        // Expandir el obstáculo con margen para evitar rozar
        QRect obstaculoExpandido = obstaculo.adjusted(
            -MARGEN_EXTRA, -MARGEN_EXTRA,
            MARGEN_EXTRA, MARGEN_EXTRA
            );

        int startX = qMax(0, obstaculoExpandido.left() / GRID_SIZE);
        int endX = qMin(GRID_WIDTH - 1, obstaculoExpandido.right() / GRID_SIZE);
        int startY = qMax(0, obstaculoExpandido.top() / GRID_SIZE);
        int endY = qMin(GRID_HEIGHT - 1, obstaculoExpandido.bottom() / GRID_SIZE);

        for (int x = startX; x <= endX; x++) {
            for (int y = startY; y <= endY; y++) {
                gridNavegacion[x][y] = false;
            }
        }
    }

    // Marcar bordes como no navegables (zona de seguridad arriba ampliada)
    for (int x = 0; x < GRID_WIDTH; x++) {
        // ✅ Ampliada zona de seguridad arriba (de 6 a 8 celdas)
        gridNavegacion[x][0] = false;
        gridNavegacion[x][1] = false;
        gridNavegacion[x][2] = false;
        gridNavegacion[x][3] = false;
        gridNavegacion[x][4] = false;
        gridNavegacion[x][5] = false;
        gridNavegacion[x][6] = false;
        gridNavegacion[x][7] = false;
        gridNavegacion[x][GRID_HEIGHT - 1] = false;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        gridNavegacion[0][y] = false;
        gridNavegacion[1][y] = false;
        gridNavegacion[GRID_WIDTH - 1][y] = false;
    }

    qDebug() << "[Automata] Grid de navegación generado con márgenes ampliados";
}

bool Automata::celdaNavegable(int gx, int gy) const
{
    if (gx < 0 || gx >= GRID_WIDTH || gy < 0 || gy >= GRID_HEIGHT) {
        return false;
    }
    return gridNavegacion[gx][gy];
}

QPoint Automata::posicionAGrid(float x, float y) const
{
    int gx = qBound(0, static_cast<int>(x / GRID_SIZE), GRID_WIDTH - 1);
    int gy = qBound(0, static_cast<int>(y / GRID_SIZE), GRID_HEIGHT - 1);
    return QPoint(gx, gy);
}

QPointF Automata::gridAPosicion(int gx, int gy) const
{
    return QPointF(gx * GRID_SIZE + GRID_SIZE / 2.0f,
                   gy * GRID_SIZE + GRID_SIZE / 2.0f);
}

// =============================================================================
// ALGORITMO A* PATHFINDING
// =============================================================================

int Automata::heuristica(QPoint a, QPoint b) const
{
    int dx = std::abs(a.x() - b.x());
    int dy = std::abs(a.y() - b.y());
    return 10 * (dx + dy) + (14 - 2 * 10) * qMin(dx, dy);
}

QVector<QPoint> Automata::obtenerVecinos(QPoint p) const
{
    QVector<QPoint> vecinos;

    // 8 direcciones: cardinales y diagonales
    const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    const int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    for (int i = 0; i < 8; i++) {
        int nx = p.x() + dx[i];
        int ny = p.y() + dy[i];

        if (celdaNavegable(nx, ny)) {
            // Para diagonales, verificar que las celdas adyacentes también sean navegables
            if (i % 2 == 1) { // Diagonal
                bool cardinalOk = celdaNavegable(p.x() + dx[i], p.y()) &&
                                  celdaNavegable(p.x(), p.y() + dy[i]);
                if (!cardinalOk) continue;
            }

            vecinos.append(QPoint(nx, ny));
        }
    }

    return vecinos;
}

QVector<QPoint> Automata::reconstruirRuta(QMap<QPoint, QPoint> &cameFrom, QPoint actual)
{
    QVector<QPoint> ruta;
    ruta.prepend(actual);

    while (cameFrom.contains(actual)) {
        actual = cameFrom[actual];
        ruta.prepend(actual);
    }

    return ruta;
}

QVector<QPoint> Automata::calcularRutaAStar(QPoint inicio, QPoint fin)
{
    // Si el inicio o fin no son navegables, buscar celda cercana navegable
    if (!celdaNavegable(inicio.x(), inicio.y())) {
        for (int r = 1; r < 5; r++) {
            for (int dx = -r; dx <= r; dx++) {
                for (int dy = -r; dy <= r; dy++) {
                    if (celdaNavegable(inicio.x() + dx, inicio.y() + dy)) {
                        inicio = QPoint(inicio.x() + dx, inicio.y() + dy);
                        goto encontrado_inicio;
                    }
                }
            }
        }
    encontrado_inicio:;
    }

    if (!celdaNavegable(fin.x(), fin.y())) {
        for (int r = 1; r < 5; r++) {
            for (int dx = -r; dx <= r; dx++) {
                for (int dy = -r; dy <= r; dy++) {
                    if (celdaNavegable(fin.x() + dx, fin.y() + dy)) {
                        fin = QPoint(fin.x() + dx, fin.y() + dy);
                        goto encontrado_fin;
                    }
                }
            }
        }
    encontrado_fin:;
    }

    // Estructura para el heap de prioridad
    struct Nodo {
        QPoint pos;
        int fScore;
        bool operator>(const Nodo &other) const { return fScore > other.fScore; }
    };

    std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> openSet;
    QMap<QPoint, QPoint> cameFrom;
    QMap<QPoint, int> gScore;
    QMap<QPoint, int> fScore;

    gScore[inicio] = 0;
    fScore[inicio] = heuristica(inicio, fin);
    openSet.push({inicio, fScore[inicio]});

    int iteraciones = 0;
    const int MAX_ITERACIONES = 1000;

    while (!openSet.empty() && iteraciones < MAX_ITERACIONES) {
        iteraciones++;
        QPoint actual = openSet.top().pos;
        openSet.pop();

        // Llegamos al destino (o cerca)
        if (actual == fin || heuristica(actual, fin) < 20) {
            return reconstruirRuta(cameFrom, actual);
        }

        for (const QPoint &vecino : obtenerVecinos(actual)) {
            // Costo: 10 para cardinal, 14 para diagonal
            int movCost = (vecino.x() != actual.x() && vecino.y() != actual.y()) ? 14 : 10;
            int tentativeG = gScore.value(actual, INT_MAX) + movCost;

            if (tentativeG < gScore.value(vecino, INT_MAX)) {
                cameFrom[vecino] = actual;
                gScore[vecino] = tentativeG;
                fScore[vecino] = tentativeG + heuristica(vecino, fin);
                openSet.push({vecino, fScore[vecino]});
            }
        }
    }

    // No se encontró ruta
    qDebug() << "[Automata] No se encontró ruta de" << inicio << "a" << fin;
    return QVector<QPoint>();
}

// =============================================================================
// MOVIMIENTO HACIA WAYPOINT - ALGORITMO MEJORADO ANTI-ATASCO
// =============================================================================

bool Automata::moverHaciaWaypoint(float deltaTime)
{
    if (rutaActual.isEmpty() || indicePuntoRuta >= rutaActual.size()) {
        return false;
    }

    QPointF destino = gridAPosicion(rutaActual[indicePuntoRuta].x(),
                                    rutaActual[indicePuntoRuta].y());

    float dx = destino.x() - posX;
    float dy = destino.y() - posY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    // Si llegamos al waypoint actual, pasar al siguiente
    if (distancia < GRID_SIZE / 2.0f) {
        indicePuntoRuta++;
        if (indicePuntoRuta >= rutaActual.size()) {
            return false; // Llegamos al final
        }

        // Actualizar destino
        destino = gridAPosicion(rutaActual[indicePuntoRuta].x(),
                                rutaActual[indicePuntoRuta].y());
        dx = destino.x() - posX;
        dy = destino.y() - posY;
        distancia = std::sqrt(dx * dx + dy * dy);
    }

    if (distancia > 0) {
        float velocidad = 3.0f;
        float movX = (dx / distancia) * velocidad;
        float movY = (dy / distancia) * velocidad;

        float nuevaX = posX + movX;
        float nuevaY = posY + movY;

        // Hitbox muy pequeño
        const int ANCHO_COL = 8;
        const int ALTO_COL = 12;

        if (!hayColision(nuevaX, nuevaY, ANCHO_COL, ALTO_COL) &&
            dentroLimites(nuevaX, nuevaY)) {
            posX = nuevaX;
            posY = nuevaY;
            actualizarDireccionSprite(movX, movY);
            intentosFallidos = 0;
            return true;
        } else {
            // SISTEMA ANTI-ATASCO MEJORADO

            // Intentar movimiento en X solo
            if (!hayColision(nuevaX, posY, ANCHO_COL, ALTO_COL) &&
                dentroLimites(nuevaX, posY)) {
                posX = nuevaX;
                actualizarDireccionSprite(movX, 0);
                return true;
            }

            // Intentar movimiento en Y solo
            if (!hayColision(posX, nuevaY, ANCHO_COL, ALTO_COL) &&
                dentroLimites(posX, nuevaY)) {
                posY = nuevaY;
                actualizarDireccionSprite(0, movY);
                return true;
            }

            // Intentar 8 direcciones de escape
            const float ESCAPE_DIST = 5.0f;
            float escapeDirs[8][2] = {
                {ESCAPE_DIST, 0}, {-ESCAPE_DIST, 0},
                {0, ESCAPE_DIST}, {0, -ESCAPE_DIST},
                {ESCAPE_DIST, ESCAPE_DIST}, {-ESCAPE_DIST, -ESCAPE_DIST},
                {ESCAPE_DIST, -ESCAPE_DIST}, {-ESCAPE_DIST, ESCAPE_DIST}
            };

            for (int i = 0; i < 8; i++) {
                float testX = posX + escapeDirs[i][0];
                float testY = posY + escapeDirs[i][1];
                if (!hayColision(testX, testY, ANCHO_COL, ALTO_COL) &&
                    dentroLimites(testX, testY)) {
                    posX = testX;
                    posY = testY;
                    return true;
                }
            }

            // Si aún está bloqueado, forzar teleport pequeño hacia el objetivo
            intentosFallidos++;
            if (intentosFallidos > 2) {
                float teleportDist = 10.0f;
                float telX = posX + (dx / distancia) * teleportDist;
                float telY = posY + (dy / distancia) * teleportDist;
                telX = qBound(80.0f, telX, 940.0f);
                telY = qBound(220.0f, telY, 720.0f);
                posX = telX;
                posY = telY;

                // Recalcular ruta
                rutaActual.clear();
                indicePuntoRuta = 0;
                intentosFallidos = 0;
                tiempoRecalculoRuta = 0;
                qDebug() << "[Automata] Teleport anti-atasco a:" << posX << posY;
            }

            return false;
        }
    }

    return true;
}

// =============================================================================
// ACTUALIZACIÓN PRINCIPAL
// =============================================================================

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
    // =========================================================================
    // CICLO DE INTELIGENCIA: PENSAR -> DECIDIR -> ACTUAR -> MEMORIZAR
    // =========================================================================

    // PRIORIDAD 1: Si tiene letra -> ACTUAR (subir y soltar)
    if (!letrasRecogidas.isEmpty() || estaBajando) {
        subirLetraArriba(deltaTime);
        return;
    }

    // PRIORIDAD 2: Si NO está activo -> quedarse quieto
    if (!automataActivo) {
        estaQuieto = true;
        direccion = FRENTE;
        return;
    }

    // =========================================================================
    // FASE 1: PENSAR - Analizar el entorno y encontrar objetivos
    // =========================================================================

    Letra* objetivo = encontrarLetraCercana(letras);
    if (!objetivo) {
        estaQuieto = true;
        direccion = FRENTE;
        rutaActual.clear();
        letraObjetivo = nullptr;
        return;
    }

    // MEMORIZAR el objetivo actual
    letraObjetivo = objetivo;
    float targetX = objetivo->getPosX();
    float targetY = objetivo->getPosY();
    float distX = targetX - posX;
    float distY = targetY - posY;
    float distancia = std::sqrt(distX * distX + distY * distY);

    // =========================================================================
    // FASE 2: DECIDIR - Evaluar si recoger o moverse
    // =========================================================================

    if (distancia <= RANGO_RECOGIDA) {
        // DECISIÓN: Recoger la letra
        qDebug() << "[Automata] DECISIÓN: Recoger letra:" << objetivo->getLetra()
                 << "en rango aumentado:" << RANGO_RECOGIDA;
        objetivo->setRecogida(true);
        agregarLetra(objetivo);
        //  Pegar letra al autómata
        letraEnMano = objetivo;
        offsetLetraX = 0.0f;
        offsetLetraY = -30.0f;


        // MEMORIZAR: Limpiar ruta
        rutaActual.clear();
        indicePuntoRuta = 0;
        intentosFallidos = 0;
        return;
    }

    // DECISIÓN: Moverse hacia el objetivo
    estaQuieto = false;

    // =========================================================================
    // FASE 3: ACTUAR - Pathfinding y movimiento (CON RUTA PARA NO CHOCAR)
    // =========================================================================

    QPoint gridObjetivo = posicionAGrid(targetX, targetY);
    QPoint gridActual = posicionAGrid(posX, posY);

    // PENSAR: ¿Necesito recalcular la ruta?
    tiempoRecalculoRuta -= deltaTime;

    bool necesitaRecalculo = rutaActual.isEmpty() ||
                             indicePuntoRuta >= rutaActual.size() ||
                             gridObjetivo != ultimoObjetivo ||
                             tiempoRecalculoRuta <= 0;

    if (necesitaRecalculo) {
        // ACTUAR: Calcular nueva ruta con A* para EVITAR CHOCAR CON MESAS
        rutaActual = calcularRutaAStar(gridActual, gridObjetivo);
        indicePuntoRuta = 0;

        // MEMORIZAR: Guardar el objetivo y resetear contadores
        ultimoObjetivo = gridObjetivo;
        tiempoRecalculoRuta = 1.5f;
        intentosFallidos = 0;

        qDebug() << "[Automata] Ruta calculada con A*:" << rutaActual.size() << "puntos";
    }

    // ACTUAR: Moverse hacia el siguiente waypoint
    bool seMovio = moverHaciaWaypoint(deltaTime);

    // =========================================================================
    // FASE 4: MEMORIZAR - Guardar estado para detectar atascos
    // =========================================================================

    float movimientoDesdeUltimo = std::sqrt(
        (posX - ultimaPosX) * (posX - ultimaPosX) +
        (posY - ultimaPosY) * (posY - ultimaPosY)
        );

    if (movimientoDesdeUltimo < 0.5f) {
        tiempoAtascado += deltaTime;
        if (tiempoAtascado > 1.0f) {
            // PENSAR: Detectado atasco, DECIDIR recalcular
            rutaActual.clear();
            tiempoAtascado = 0.0f;
            qDebug() << "[Automata] MEMORIA: Atasco detectado, recalculando...";
        }
    } else {
        tiempoAtascado = 0.0f;
    }

    // MEMORIZAR posición actual para próxima iteración
    ultimaPosX = posX;
    ultimaPosY = posY;

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
    rutaActual.clear();
    indicePuntoRuta = 0;
    tiempoAtascado = 0.0f;
    intentosFallidos = 0;
}

void Automata::subirLetraArriba(float deltaTime)
{
    const float TARGET_Y_ARRIBA = 250.0f;
    const float VELOCIDAD_VERTICAL = 100.0f;

    // SUBIENDO - sin colisiones
    if (posY > TARGET_Y_ARRIBA) {
        posY -= VELOCIDAD_VERTICAL * deltaTime;

        //Si tiene letra pegada, hacerla seguir
        if (letraEnMano != nullptr && letraEnMano->estaRecogida()) {
            letraEnMano->setPosicion(posX + offsetLetraX, posY + offsetLetraY);
        }

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
            float soltarX = posX + (QRandomGenerator::global()->bounded(20) - 10);
            float soltarY = posY - 30.0f;
            soltarX = qBound(100.0f, soltarX, 920.0f);
            letra->setSoltadaSinMovimiento(soltarX, soltarY);
            letrasRecogidas.removeAll(letra);
            qDebug() << "[Automata] Letra soltada cerca:" << letra->getLetra()
                     << "en X:" << soltarX << "Y:" << soltarY;
        }

        // DESACTIVAR COMPLETAMENTE
        letraEnMano = nullptr;
        automataActivo = false;
        tieneMision = false;
        estaBajando = false;
        estaQuieto = true;
        rutaActual.clear();
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
    // Si está quieto, forzar dirección FRENTE
    if (estaQuieto && letrasRecogidas.isEmpty()) {
        direccion = FRENTE;
    }

    QPixmap spriteDir = getSpriteActual();

    if (spriteDir.isNull()) {
        if (letrasRecogidas.isEmpty() && posY > 300.0f && estaQuieto) {
            painter.fillRect(posX - 20, posY - 20, 40, 40, QColor(0, 100, 255, 150));
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(posX - 15, posY + 5, "NPC");
        }
        return;
    }

    const int numFrames = 2;
    int frameWidth = spriteDir.width() / numFrames;
    int frameHeight = spriteDir.height();
    QRect src(frameAnimacion * frameWidth, 0, frameWidth, frameHeight);

    int drawWidth = (frameWidth / 2) * 0.8;
    int drawHeight = (frameHeight / 2) * 0.8;
    int offsetX = posX - drawWidth / 2;
    int offsetY = posY - drawHeight;

    if (frameAnimacion == 1) {
        offsetX += drawWidth * 0.3;
    }

    QRect dst(offsetX, offsetY, drawWidth, drawHeight);
    painter.drawPixmap(dst, spriteDir, src);

    // DEBUG: Mostrar si está activo
    if (automataActivo) {
        painter.setPen(Qt::green);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(posX - 20, posY - 45, "ACTIVO");
    }
}

bool Automata::hayColision(float x, float y, int ancho, int alto)
{
    const int MARGEN_COLISION = 5;
    QRect rect(x - ancho/2, y - alto/2, ancho, alto);

    for (const QRect &mesa : colisiones) {
        QRect mesaConMargen = mesa.adjusted(-MARGEN_COLISION, -MARGEN_COLISION,
                                            MARGEN_COLISION, MARGEN_COLISION);
        if (rect.intersects(mesaConMargen)) {
            return true;
        }
    }

    return false;
}
