#include "Nivel1.h"
#include "ResourceManager.h"
#include "DifficultyManager.h"
#include "SoundManager.h"
#include <QDebug>
#include <QRandomGenerator>
#include <cmath>

const float Nivel1::TIEMPO_LIMITE = 90.0f;
const float Nivel1::DURACION_ACCION_IMPRENTA = 2.0f;

Nivel1::Nivel1(QObject *parent)
    : QObject(parent),
    jugador(nullptr),
    automata(nullptr),
    imprentaPosX(500.0f),
    imprentaPosY(360.0f),
    imprentaAncho(600.0f),
    imprentaAlto(500.0f),
    tiempoRestante(TIEMPO_LIMITE),
    juegoActivo(false),
    letraActualIndex(0),
    puntuacion(0),
    tiempoAccionImprenta(0.0f),
    imprentaEnAccion(false)
{
    qDebug() << "\n========== [Nivel1] Inicializando ==========";
    try {
        cargarSpritesObjetos();

        // Crear jugador
        jugador = new Jugador(this);
        if (!jugador) {
            throw std::runtime_error("No se pudo crear el jugador");
        }

        jugador->setLimites(1024.0f, 800.0f);
        // Usar DifficultyManager para configurar velocidad
        jugador->setVelocidadMovimiento(DifficultyManager::getInstance().getVelocidadJugador());

        // Definir áreas
        definirAreaMovimiento();
        definirColisiones();

        // Crear automata
        automata = new Automata(this);
        if (!automata) {
            throw std::runtime_error("No se pudo crear el autómata");
        }

        automata->setLimites(1024.0f, 800.0f);
        automata->cargarSprites();
        automata->setColisiones(colisiones);
        automata->setAreaMovimiento(QRect(0, 0, 1024, 800));
        automata->setVelocidadMovimiento(DifficultyManager::getInstance().getVelocidadAutomata());

        // Crear letras
        crearLetras();

        // Cargar fondo
        fondoOriginal = ResourceManager::getInstance().getSprite("fondo_nivel1");
        if (!fondoOriginal.isNull()) {
            fondoEscalado = fondoOriginal.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            qDebug() << "[Nivel1] Fondo cargado exitosamente:" << fondoEscalado.size();
        } else {
            qWarning() << "[Nivel1] No se pudo cargar el fondo_nivel1";
        }

        // Iniciar juego
        iniciarJuego();

    } catch (const std::exception& e) {
        qCritical() << "[Nivel1] EXCEPCIÓN en constructor:" << e.what();
        juegoActivo = false;
    }

    qDebug() << "[Nivel1] Área de movimiento:" << areaMovimiento;
    qDebug() << "[Nivel1] Colisiones definidas:" << colisiones.size();
    qDebug() << "[Nivel1] Dificultad:" << DifficultyManager::getInstance().getNombreDificultad();
    qDebug() << "========== [Nivel1] Inicializado ==========\n";
}

Nivel1::~Nivel1()
{
    // Limpiar letras
    for (Letra* letra : letras) {
        delete letra;
    }
    letras.clear();
    qDebug() << "[Nivel1] Destruido";
}

void Nivel1::iniciarJuego()
{
    QVector<QString> palabrasDisponibles = {
        "GUTENBERG",
        "IMPRENTA",
        "LETRA",
        "PRENSA",
        "LIBRO"
    };

    int indiceAleatorio = QRandomGenerator::global()->bounded(palabrasDisponibles.size());
    palabraObjetivo = palabrasDisponibles[indiceAleatorio];
    letraActualIndex = 0;

    // Usar tiempo límite según dificultad
    tiempoRestante = DifficultyManager::getInstance().getTiempoLimite();
    juegoActivo = true;
    puntuacion = 0;

    qDebug() << "[Nivel1] JUEGO INICIADO - Palabra objetivo:" << palabraObjetivo;
    qDebug() << "[Nivel1] Primera letra a recoger:" << palabraObjetivo[0];
    qDebug() << "[Nivel1] Tiempo límite:" << tiempoRestante << "segundos";
}

void Nivel1::actualizarJuego(float deltaTime)
{
    if (!juegoActivo) return;

    tiempoRestante -= deltaTime;
    if (tiempoRestante <= 0.0f) {
        tiempoRestante = 0.0f;
        juegoActivo = false;
        qDebug() << "[Nivel1] GAME OVER - Tiempo agotado";
    }

    // Actualizar tiempo de acción de la imprenta
    if (imprentaEnAccion) {
        tiempoAccionImprenta -= deltaTime;
        if (tiempoAccionImprenta <= 0.0f) {
            tiempoAccionImprenta = 0.0f;
            imprentaEnAccion = false;
        }
    }
}

void Nivel1::verificarLetraEntregada()
{
    if (!juegoActivo || !jugador) return;

    auto letrasRecogidas = jugador->getLetrasRecogidas();
    if (letrasRecogidas.isEmpty()) return;

    Letra *letraRecogida = letrasRecogidas[0];
    QChar letraEsperada = palabraObjetivo[letraActualIndex];

    if (letraRecogida->getLetra() == letraEsperada.toLatin1()) {
        qDebug() << "[Nivel1] ¡CORRECTO! Letra:" << letraRecogida->getLetra();
        jugador->soltarLetra(letraRecogida);
        letraRecogida->setSoltada(imprentaPosX, imprentaPosY);

        // ACCIONAR LA IMPRENTA
        imprentaEnAccion = true;
        tiempoAccionImprenta = DURACION_ACCION_IMPRENTA;
        
        // Reproducir sonido de la prensa
        SoundManager::getInstance().playSoundPrensa();

        letraActualIndex++;
        puntuacion += DifficultyManager::getInstance().getPuntosPorLetra();

        if (letraActualIndex >= palabraObjetivo.length()) {
            juegoActivo = false;
            DifficultyManager::getInstance().registrarRondaCompletada();
            qDebug() << "[Nivel1] ¡¡VICTORIA!! Palabra completada:" << palabraObjetivo;
            qDebug() << "[Nivel1] Puntuación:" << puntuacion;
        } else {
            qDebug() << "[Nivel1] Siguiente letra:" << palabraObjetivo[letraActualIndex];
        }
    } else {
        qDebug() << "[Nivel1] ¡INCORRECTO! Esperaba:" << letraEsperada
                 << "Entregaste:" << letraRecogida->getLetra();
        puntuacion -= DifficultyManager::getInstance().getPenalizacionError();
    }
}

void Nivel1::cargarSpritesObjetos()
{
    auto &rm = ResourceManager::getInstance();
    spriteImprentaBase = rm.getSprite("nivel1_prensa_imprenta");
    spriteImprentaAccionada = rm.getSprite("nivel1_prensa_imprenta2");
    spriteMesaArriba = rm.getSprite("nivel1_mesa_arriba");
    spriteMesaAbajo = rm.getSprite("nivel1_mesa_abajo");
    spriteLetras = rm.getSprite("nivel1_letras");

    qDebug() << "[Nivel1] Sprites de objetos cargados:";
    qDebug() << " - Imprenta Base:" << spriteImprentaBase.size();
    qDebug() << " - Imprenta Accionada:" << spriteImprentaAccionada.size();
    qDebug() << " - Mesa Arriba:" << spriteMesaArriba.size();
    qDebug() << " - Letras:" << spriteLetras.size();
}

void Nivel1::definirAreaMovimiento()
{
    areaMovimiento = QRect(80, 200, 860, 700);
}

void Nivel1::definirColisiones()
{
    colisiones.append(QRect(120, 300, 180, 120)); // Mesa 1
    colisiones.append(QRect(650, 300, 220, 120)); // Mesa 2
    colisiones.append(QRect(420, 300, 120, 222)); // Mesa 3
    colisiones.append(QRect(120, 540, 180, 140)); // Mesa 4
    colisiones.append(QRect(650, 540, 220, 140)); // Mesa 5
    colisiones.append(QRect(370, 690, 212, 120)); // Mesa 6

    qDebug() << "[Nivel1] Colisiones de mesas definidas:" << colisiones.size();
}

bool Nivel1::hayColision(float x, float y, int ancho, int alto)
{
    QRect jugadorRect(x, y, ancho, alto);
    for (const QRect &mesa : colisiones) {
        if (jugadorRect.intersects(mesa)) {
            return true;
        }
    }
    return false;
}

void Nivel1::intentarRecogerLetra()
{
    if (!jugador) return;

    if (!jugador->getLetrasRecogidas().isEmpty()) {
        qDebug() << "[Nivel1] Ya llevas una letra, suéltala primero con R";
        return;
    }

    float jugadorX = jugador->getPosX();
    float jugadorY = jugador->getPosY();
    const float RANGO_INTERACCION = DifficultyManager::getInstance().getRangoInteraccion();

    for (Letra *letra : letras) {
        if (!letra->estaRecogida()) {
            float distancia = jugador->distanciaA(letra);
            if (distancia <= RANGO_INTERACCION) {
                letra->setRecogida(true);
                jugador->agregarLetra(letra);
                qDebug() << "[Nivel1] Letra recogida:" << letra->getLetra();
                return;
            }
        }
    }
}

void Nivel1::intentarRecogerLetraConClick(float mouseX, float mouseY)
{
    if (!jugador) return;

    if (!jugador->getLetrasRecogidas().isEmpty()) {
        return;
    }

    float jugadorX = jugador->getPosX();
    float jugadorY = jugador->getPosY();
    const float RANGO_INTERACCION = DifficultyManager::getInstance().getRangoInteraccion();

    float distXJugador = jugadorX - mouseX;
    float distYJugador = jugadorY - mouseY;
    float distanciaClick = sqrt(distXJugador * distXJugador + distYJugador * distYJugador);

    if (distanciaClick > RANGO_INTERACCION) {
        return;
    }

    Letra *letraCercana = nullptr;
    float menorDistancia = RANGO_INTERACCION;

    for (Letra *letra : letras) {
        if (!letra->estaRecogida()) {
            float distX = mouseX - letra->getPosX();
            float distY = mouseY - letra->getPosY();
            float distancia = sqrt(distX * distX + distY * distY);

            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                letraCercana = letra;
            }
        }
    }

    if (letraCercana) {
        letraCercana->setRecogida(true);
        jugador->agregarLetra(letraCercana);
        qDebug() << "[Nivel1] Letra recogida con click:" << letraCercana->getLetra();
    }
}

void Nivel1::soltarUltimaLetra()
{
    if (!jugador) return;

    auto letrasRecogidas = jugador->getLetrasRecogidas();
    if (!letrasRecogidas.isEmpty()) {
        Letra *ultimaLetra = letrasRecogidas.last();
        jugador->soltarLetra(ultimaLetra);

        float offsetX = (QRandomGenerator::global()->bounded(61) - 30);
        float offsetY = (QRandomGenerator::global()->bounded(61) - 30);
        ultimaLetra->setSoltada(jugador->getPosX() + offsetX, jugador->getPosY() + offsetY);

        qDebug() << "[Nivel1] Letra soltada:" << ultimaLetra->getLetra();
    }
}

void Nivel1::intentarInteractuarConAutomata()
{
    if (!jugador || !automata) return;

    float distancia = jugador->distanciaA(automata);
    const float RANGO_INTERACCION_AUTOMATA = 150.0f;

    if (distancia <= RANGO_INTERACCION_AUTOMATA) {
        qDebug() << "[Nivel1] ¡Jugador interactuó con el autómata!";

        if (!juegoActivo || letraActualIndex >= palabraObjetivo.length()) {
            qDebug() << "[Nivel1] No hay más letras que buscar";
            return;
        }

        char letraFaltante = palabraObjetivo[letraActualIndex].toLatin1();
        qDebug() << "[Nivel1] La letra faltante es:" << letraFaltante;
        automata->interactuarConJugador(letraFaltante);

    } else {
        qDebug() << "[Nivel1] Acércate más al autómata.";
    }
}

void Nivel1::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    actualizarJuego(deltaTime);

    if (jugador) {
        float posXAntes = jugador->getPosX();
        float posYAntes = jugador->getPosY();

        jugador->actualizar(arriba, abajo, izquierda, derecha, deltaTime);

        float jugadorX = jugador->getPosX();
        float jugadorY = jugador->getPosY();

        const int ANCHO_JUGADOR = 32;
        const int ALTO_JUGADOR = 48;

        // Actualizar física de letras
        for (Letra *letra : letras) {
            letra->actualizar(deltaTime);
        }

        // Si la letra está recogida, muévela con el jugador
        auto letrasRecogidas = jugador->getLetrasRecogidas();
        if (!letrasRecogidas.isEmpty()) {
            Letra *letra = letrasRecogidas[0];
            letra->setPosicion(jugador->getPosX(), jugador->getPosY() - 60);
        }

        // Actualizar automata
        automata->actualizar(deltaTime, letras);

        // Colisión con mesas
        if (hayColision(jugadorX, jugadorY, ANCHO_JUGADOR, ALTO_JUGADOR)) {
            jugador->setPosicion(posXAntes, posYAntes);
            jugadorX = posXAntes;
            jugadorY = posYAntes;
        }

        // Límites del área de movimiento
        if (jugadorX < areaMovimiento.x()) {
            jugador->setPosicion(areaMovimiento.x(), jugadorY);
        } else if (jugadorX + ANCHO_JUGADOR > areaMovimiento.x() + areaMovimiento.width()) {
            jugador->setPosicion(areaMovimiento.x() + areaMovimiento.width() - ANCHO_JUGADOR, jugadorY);
        }

        if (jugadorY < areaMovimiento.y()) {
            jugador->setPosicion(jugadorX, areaMovimiento.y());
        } else if (jugadorY + ALTO_JUGADOR > areaMovimiento.y() + areaMovimiento.height()) {
            jugador->setPosicion(jugadorX, areaMovimiento.y() + areaMovimiento.height() - ALTO_JUGADOR);
        }
    }
}

void Nivel1::crearLetras()
{
    try {
        const char letrasDisponibles[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y'
        };

        // ===== POSICIONES INICIALES SEGURAS (SOBRE LAS MESAS O ESPACIOS SEGUROS) =====
        // Las letras CAERÁN DESDE ARRIBA Y REBOTAN EN ESTAS POSICIONES
        QVector<std::pair<float, float>> posicionesSeguras = {
            // FILA 1 - Zona superior (sobre mesa 1)
            {160.0f, 330.0f}, {240.0f, 320.0f},

            // FILA 2 - Sobre mesa 2
            {700.0f, 330.0f}, {800.0f, 320.0f},

            // FILA 3 - Sobre mesa 3 (centro)
            {450.0f, 330.0f}, {480.0f, 340.0f},

            // FILA 4 - Espacio libre izquierda
            {100.0f, 380.0f}, {250.0f, 390.0f},

            // FILA 5 - Espacio libre derecha
            {850.0f, 380.0f}, {920.0f, 390.0f},

            // FILA 6 - Espacio libre centro
            {380.0f, 410.0f}, {520.0f, 420.0f},

            // FILA 7 - Espacio libre bajo (sobre mesa 4)
            {170.0f, 580.0f}, {220.0f, 590.0f},

            // FILA 8 - Espacio libre bajo (sobre mesa 5)
            {730.0f, 580.0f}, {820.0f, 590.0f},

            // FILA 9 - Espacio libre bajo (sobre mesa 6)
            {420.0f, 630.0f}, {480.0f, 640.0f},

            // Posiciones adicionales de relleno (espacios libres)
            {300.0f, 460.0f}, {650.0f, 470.0f},
            {200.0f, 500.0f}, {850.0f, 510.0f},
            {400.0f, 360.0f}, {600.0f, 370.0f},
            {150.0f, 440.0f}, {900.0f, 450.0f}
        };

        int numLetras = DifficultyManager::getInstance().getNumeroLetras();
        if (numLetras > NUM_LETRAS_TOTALES) numLetras = NUM_LETRAS_TOTALES;

        for (int i = 0; i < numLetras; i++) {
            Letra *letra = new Letra(this);
            if (!letra) {
                throw std::bad_alloc();
            }

            letra->setLetra(letrasDisponibles[i % (sizeof(letrasDisponibles) / sizeof(letrasDisponibles[0]))]);

            // Obtener posición segura
            auto posicionSegura = posicionesSeguras[i % posicionesSeguras.size()];

            // Agregar variación pequeña
            float posicionX = posicionSegura.first + (QRandomGenerator::global()->bounded(21) - 10);
            float posicionY = posicionSegura.second + (QRandomGenerator::global()->bounded(21) - 10);

            // Asegurar que está dentro de los límites
            posicionX = qBound(85.0f, posicionX, 935.0f);
            posicionY = qBound(320.0f, posicionY, 720.0f);

            // ===== CONFIGURAR CAÍDA DESDE ARRIBA =====
            // La letra empieza desde arriba (Y: 150) y cae hasta su posición segura
            letra->setPosicion(posicionX, 150.0f + QRandomGenerator::global()->bounded(50));
            letra->setTamanio(50, 50);

            // El piso es la posición donde debería caer y rebotar
            float pisoDestino = posicionY;
            letra->iniciarCaida(pisoDestino);

            letras.append(letra);

            qDebug() << "[Nivel1] Letra" << i << ":" << letra->getLetra()
                     << "caerá hasta Y:" << pisoDestino;
        }

        qDebug() << "[Nivel1] Total de letras creadas:" << letras.size();

    } catch (const std::bad_alloc& e) {
        qCritical() << "[Nivel1] Error de memoria al crear letras:" << e.what();
        juegoActivo = false;
    }
}

void Nivel1::dibujarGameUI(QPainter &painter)
{
    painter.fillRect(0, 0, 1024, 75, QColor(0, 0, 0, 150));
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(Qt::white);

    painter.drawText(10, 22, "Palabra: " + palabraObjetivo);

    // Mostrar dificultad
    painter.setPen(Qt::cyan);
    painter.drawText(200, 22, "[" + DifficultyManager::getInstance().getNombreDificultad() + "]");

    if (letraActualIndex < palabraObjetivo.length()) {
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.setPen(Qt::yellow);
        painter.drawText(450, 22, "BUSCA: " + QString(palabraObjetivo[letraActualIndex]));
    }

    if (jugador) {
        auto letrasRecogidas = jugador->getLetrasRecogidas();
        if (!letrasRecogidas.isEmpty()) {
            Letra *letraRecogida = letrasRecogidas[0];
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.setPen(Qt::cyan);
            painter.drawText(450, 50, "Llevas: " + QString(letraRecogida->getLetra()));
        }
    }

    painter.setFont(QFont("Arial", 12, QFont::Bold));
    int seg = (int)tiempoRestante;
    if (tiempoRestante <= 10.0f) {
        painter.setPen(Qt::red);
    } else {
        painter.setPen(Qt::green);
    }
    painter.drawText(850, 22, "Tiempo: " + QString::number(seg) + "s");

    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::cyan);
    QString progreso = "";
    for (int i = 0; i < palabraObjetivo.length(); i++) {
        if (i < letraActualIndex) {
            progreso += "[" + QString(palabraObjetivo[i]) + "]";
        } else if (i == letraActualIndex) {
            progreso += "(" + QString(palabraObjetivo[i]) + ")";
        } else {
            progreso += "_";
        }
    }
    painter.drawText(10, 50, progreso);

    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::white);
    painter.drawText(850, 50, "Pts: " + QString::number(puntuacion));

    if (!juegoActivo) {
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.setPen(Qt::red);
        if (letraActualIndex >= palabraObjetivo.length()) {
            painter.drawText(300, 400, "¡¡VICTORIA!!");
        } else {
            painter.drawText(350, 400, "GAME OVER");
        }
    }
}

void Nivel1::dibujarFondo(QPainter &painter)
{
    if (!fondoEscalado.isNull()) {
        painter.drawPixmap(0, 0, fondoEscalado);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(139, 90, 43));
    }
}

void Nivel1::dibujarUI(QPainter &painter)
{
    painter.setFont(QFont("Courier", 10));
    painter.setPen(Qt::white);
    painter.drawText(10, 750, "WASD/Flechas: Mover | R: Recoger/Soltar | E: Entregar | Click: Recoger");
}

void Nivel1::dibujar(QPainter &painter)
{
    dibujarFondo(painter);

    QVector<ObjetoConProfundidad> elementos;

    elementos.append({0, 360.0f, 0});
    elementos.append({1, imprentaPosY, 0});
    elementos.append({4, 640.0f, 0});

    float yMesaCentro = 740.0f;
    if (jugador) {
        float jugadorX = jugador->getPosX();
        if ((jugadorX > 80.0f && jugadorX < 303.0f) || (jugadorX > 615.0f && jugadorX < 874.0f)) {
            yMesaCentro = 600.0f;
        }
    }

    elementos.append({3, yMesaCentro, 0});

    for (Letra *letra : letras) {
        elementos.append({5, letra->getPosY(), 0});
    }

    if (jugador) {
        elementos.append({2, jugador->getPosY(), 0});
    }

    if (automata) {
        elementos.append({6, automata->getPosY(), 0});
    }

    std::sort(elementos.begin(), elementos.end(),
              [](const ObjetoConProfundidad &a, const ObjetoConProfundidad &b) {
                  return a.y < b.y;
              });

    for (const ObjetoConProfundidad &elem : elementos) {
        if (elem.tipo == 0) {
            if (!spriteMesaArriba.isNull()) {
                QPixmap mesaEscalada = spriteMesaArriba.scaled(1024, 768,
                                                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(0, 0, mesaEscalada);
            }
        } else if (elem.tipo == 1) {
            QPixmap spriteActual = getSpriteImprentaActual();
            if (!spriteActual.isNull()) {
                QPixmap imprentaEscalada = spriteActual.scaled((int)imprentaAncho, (int)imprentaAlto,
                                                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(imprentaPosX - imprentaEscalada.width() / 2,
                                   imprentaPosY - imprentaEscalada.height() / 2, imprentaEscalada);
            }
        } else if (elem.tipo == 2) {
            if (jugador) {
                jugador->dibujar(painter);
            }
        } else if (elem.tipo == 3) {
            if (!spriteMesaAbajo.isNull()) {
                QPixmap mesaEscalada = spriteMesaAbajo.scaled(1024, 768,
                                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(0, 0, mesaEscalada);
            }
        } else if (elem.tipo == 4) {
            if (!spriteMesaAbajo.isNull()) {
                QPixmap mesaEscalada = spriteMesaAbajo.scaled(1024, 768,
                                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(0, 0, mesaEscalada);
            }
        } else if (elem.tipo == 5) {
            for (Letra *letra : letras) {
                if (letra->getPosY() == elem.y) {
                    letra->dibujar(painter, spriteLetras);
                    break;
                }
            }
        } else if (elem.tipo == 6) {
            if (automata) {
                automata->dibujar(painter);
            }
        }
    }

    dibujarGameUI(painter);
    dibujarUI(painter);
}

QPixmap Nivel1::getSpriteImprentaActual() const
{
    if (imprentaEnAccion && tiempoAccionImprenta > 0.0f) {
        return spriteImprentaAccionada;
    }
    return spriteImprentaBase;
}
