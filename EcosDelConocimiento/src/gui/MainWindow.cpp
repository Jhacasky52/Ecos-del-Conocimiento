#include "MainWindow.h"
#include "Nivel1.h"
#include "Nivel2.h"
#include "Nivel3.h"
#include "ResourceManager.h"
#include "DifficultyManager.h"
#include "SoundManager.h"
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QLinearGradient>
#include <cmath>

MainWindow::MainWindow(int nivelInicial, QWidget *parent)
    : QWidget(parent), 
      nivel1(nullptr), 
      nivel2(nullptr), 
      nivel3(nullptr),
      nivelActual(nivelInicial),
      nivelMaximoDesbloqueado(nivelInicial),
      timer(nullptr),
      estadoJuego(JUGANDO),
      puntajeNivel1(0),
      puntajeNivel2(0),
      puntajeNivel3(0),
      nivel1Completado(false),
      nivel2Completado(false),
      nivel3Completado(false),
      hoverSound(false),
      hoverAvanzar(false),
      hoverSalirVictoria(false),
      hoverContinuar(false),
      hoverSalirPausa(false),
      hoverMenuDerrota(false),
      hoverSalirDerrota(false),
      hoverMenuFinal(false),
      hoverSalirFinal(false)
{
    QString tituloNivel;
    switch (nivelInicial) {
        case 1: tituloNivel = "Taller de Gutenberg"; break;
        case 2: tituloNivel = "Biblioteca de Alejandría"; break;
        case 3: tituloNivel = "Revolución Digital"; break;
        default: tituloNivel = "Nivel " + QString::number(nivelInicial); break;
    }
    
    setWindowTitle("Ecos del Conocimiento - " + tituloNivel);
    setFixedSize(1024, 768);
    setStyleSheet("background-color: black;");
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Cargar fondos de pantallas de estado
    cargarFondosEstado();
    
    // Cargar sprites de sonido
    cargarSpritesSonido();
    
    // Definir botones de estado
    definirBotonesEstado();

    // Inicializar el nivel
    inicializarNivel(nivelInicial);
    
    // Iniciar música del nivel
    SoundManager::getInstance().playLevelMusic(nivelInicial);

    // Configurar timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16); // ~60 FPS

    qDebug() << "[MainWindow] Juego iniciado - Nivel" << nivelInicial;
}

MainWindow::~MainWindow()
{
    SoundManager::getInstance().stopBackgroundMusic();
    limpiarNiveles();
    if (timer) delete timer;
}

void MainWindow::cargarFondosEstado()
{
    // Cargar imágenes de victoria y derrota
    fondoVictoria = QPixmap(":/assets/fondos/victoria.png");
    fondoDerrota = QPixmap(":/assets/fondos/derrota.png");
    fondoPausa = QPixmap(":/assets/fondos/menu_opciones.jpg");
    
    // Escalar
    if (!fondoVictoria.isNull()) {
        fondoVictoria = fondoVictoria.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    if (!fondoDerrota.isNull()) {
        fondoDerrota = fondoDerrota.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    if (!fondoPausa.isNull()) {
        fondoPausa = fondoPausa.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "[MainWindow] Fondos de estado cargados";
}

void MainWindow::cargarSpritesSonido()
{
    spriteSoundOn = QPixmap(":/assets/fondos/sound_on.png");
    spriteSoundOff = QPixmap(":/assets/fondos/sound_off.png");
    
    // Escalar los sprites a un tamaño razonable (48x48)
    if (!spriteSoundOn.isNull()) {
        spriteSoundOn = spriteSoundOn.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!spriteSoundOff.isNull()) {
        spriteSoundOff = spriteSoundOff.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "[MainWindow] Sprites de sonido cargados";
}

void MainWindow::definirBotonesEstado()
{
    int btnWidth = 180;
    int btnHeight = 50;
    
    // Botón de sonido (arriba a la derecha)
    btnSound = QRect(960, 10, 48, 48);
    
    // Botones de pantalla de victoria (abajo izquierda y derecha)
    btnSalirVictoria = QRect(50, 680, btnWidth, btnHeight);      // Abajo izquierda = MENÚ
    btnAvanzar = QRect(794, 680, btnWidth, btnHeight);           // Abajo derecha = AVANZAR
    
    // Botones de pantalla de pausa (centrados)
    int centerX = (1024 - btnWidth) / 2;
    btnContinuar = QRect(centerX - 100, 400, btnWidth, btnHeight);
    btnSalirPausa = QRect(centerX + 100, 400, btnWidth, btnHeight);
    
    // Botones de pantalla de derrota (abajo izquierda y derecha)
    btnMenuDerrota = QRect(50, 680, btnWidth, btnHeight);        // Abajo izquierda = MENÚ
    btnSalirDerrota = QRect(794, 680, btnWidth, btnHeight);      // Abajo derecha = SALIR
    
    // Botones de resumen final
    btnMenuFinal = QRect(50, 680, btnWidth, btnHeight);
    btnSalirFinal = QRect(794, 680, btnWidth, btnHeight);
}

void MainWindow::limpiarNiveles()
{
    if (nivel1) { delete nivel1; nivel1 = nullptr; }
    if (nivel2) { delete nivel2; nivel2 = nullptr; }
    if (nivel3) { delete nivel3; nivel3 = nullptr; }
}

void MainWindow::inicializarNivel(int nivel)
{
    limpiarNiveles();
    nivelActual = nivel;
    estadoJuego = JUGANDO;
    
    QString tituloNivel;
    
    switch (nivel) {
        case 1:
            nivel1 = new Nivel1(this);
            tituloNivel = "Taller de Gutenberg";
            qDebug() << "[MainWindow] Nivel 1 inicializado";
            break;
        case 2:
            nivel2 = new Nivel2(this);
            tituloNivel = "Biblioteca de Alejandría";
            qDebug() << "[MainWindow] Nivel 2 inicializado";
            break;
        case 3:
            nivel3 = new Nivel3(this);
            tituloNivel = "Revolución Digital";
            qDebug() << "[MainWindow] Nivel 3 inicializado";
            break;
        default:
            qWarning() << "[MainWindow] Nivel no válido:" << nivel;
            nivel1 = new Nivel1(this);
            nivelActual = 1;
            tituloNivel = "Taller de Gutenberg";
            break;
    }
    
    // Cambiar música del nivel
    SoundManager::getInstance().playLevelMusic(nivelActual);
    
    // Reproducir música del nivel
    SoundManager::getInstance().playLevelMusic(nivelActual);
    
    setWindowTitle("Ecos del Conocimiento - " + tituloNivel);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int key = event->key();
    
    // Tecla ESC - Pausar/Despausar
    if (key == Qt::Key_Escape) {
        if (estadoJuego == JUGANDO) {
            pausarJuego();
        } else if (estadoJuego == PAUSADO) {
            continuarJuego();
        }
        return;
    }
    
    // Solo procesar teclas de juego si estamos jugando
    if (estadoJuego != JUGANDO) return;

    teclasPresionadas.insert(key);

    // Tecla R - Recoger/Soltar
    if (key == Qt::Key_R) {
        switch (nivelActual) {
            case 1:
                if (nivel1 && nivel1->getJugador()) {
                    auto letras = nivel1->getJugador()->getLetrasRecogidas();
                    if (!letras.isEmpty()) {
                        nivel1->soltarUltimaLetra();
                    } else {
                        nivel1->intentarRecogerLetra();
                    }
                }
                break;
            case 2:
                if (nivel2) {
                    nivel2->intentarRecogerPergamino();
                }
                break;
            case 3:
                if (nivel3 && nivel3->getJugador()) {
                    auto datos = nivel3->getJugador()->getLetrasRecogidas();
                    if (!datos.isEmpty()) {
                        nivel3->soltarUltimoDato();
                    } else {
                        nivel3->intentarRecogerDato();
                    }
                }
                break;
        }
        return;
    }

    // Tecla E - Entregar e interactuar
    if (key == Qt::Key_E) {
        switch (nivelActual) {
            case 1:
                if (nivel1 && nivel1->getJugador()) {
                    nivel1->intentarInteractuarConAutomata();
                    
                    float jugadorX = nivel1->getJugador()->getPosX();
                    float jugadorY = nivel1->getJugador()->getPosY();
                    float distX = jugadorX - 500.0f;
                    float distY = jugadorY - 360.0f;
                    float distancia = sqrt(distX * distX + distY * distY);
                    if (distancia <= 250.0f) {
                        nivel1->verificarLetraEntregada();
                    }
                }
                break;
            case 2:
                if (nivel2) {
                    nivel2->entregarPergamino();
                }
                break;
            case 3:
                if (nivel3 && nivel3->getJugador()) {
                    nivel3->intentarInteractuarConAutomata();
                    nivel3->verificarDatoEntregado();
                }
                break;
        }
        return;
    }
    
    // Tecla F - Tomar agua de la fuente (Nivel 2)
    if (key == Qt::Key_F && nivelActual == 2) {
        if (nivel2) nivel2->intentarTomarAgua();
        return;
    }
    
    // Tecla T - Lanzar cubeta (Nivel 2)
    if (key == Qt::Key_T && nivelActual == 2) {
        if (nivel2) nivel2->lanzarCubeta();
        return;
    }
    
    // Tecla G - Soltar pergamino (Nivel 2)
    if (key == Qt::Key_G && nivelActual == 2) {
        if (nivel2) nivel2->soltarPergamino();
        return;
    }
    
    // Tecla Q - Escudo (Nivel 3)
    if (key == Qt::Key_Q && nivelActual == 3) {
        if (nivel3) nivel3->activarEscudo();
        return;
    }
    
    // Espacio o W - Saltar (Nivel 2)
    if ((key == Qt::Key_Space || key == Qt::Key_W) && nivelActual == 2) {
        if (nivel2) nivel2->intentarSaltar();
        return;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    teclasPresionadas.remove(event->key());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    
    int x = event->position().x();
    int y = event->position().y();
    
    // Verificar click en botón de sonido (siempre disponible)
    if (btnSound.contains(x, y)) {
        toggleSonido();
        return;
    }

    switch (estadoJuego) {
        case PAUSADO:
            if (btnContinuar.contains(x, y)) {
                continuarJuego();
            } else if (btnSalirPausa.contains(x, y)) {
                volverAlMenuPrincipal();
            }
            break;
            
        case VICTORIA:
            if (btnAvanzar.contains(x, y)) {
                avanzarSiguienteNivel();
            } else if (btnSalirVictoria.contains(x, y)) {
                volverAlMenuPrincipal();
            }
            break;
            
        case DERROTA:
            if (btnMenuDerrota.contains(x, y)) {
                volverAlMenuPrincipal();
            } else if (btnSalirDerrota.contains(x, y)) {
                salirDelJuego();
            }
            break;
            
        case RESUMEN_FINAL:
            if (btnMenuFinal.contains(x, y)) {
                volverAlMenuPrincipal();
            } else if (btnSalirFinal.contains(x, y)) {
                salirDelJuego();
            }
            break;
            
        case JUGANDO:
            {
                float mouseX = event->position().x();
                float mouseY = event->position().y();
                
                switch (nivelActual) {
                    case 1:
                        if (nivel1) nivel1->intentarRecogerLetraConClick(mouseX, mouseY);
                        break;
                    case 3:
                        if (nivel3) nivel3->intentarRecogerDatoConClick(mouseX, mouseY);
                        break;
                }
            }
            break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->position().x();
    int y = event->position().y();
    
    // Verificar hover del botón de sonido siempre
    verificarHoverSound(x, y);

    switch (estadoJuego) {
        case PAUSADO:
            verificarHoverPausa(x, y);
            break;
        case VICTORIA:
            verificarHoverVictoria(x, y);
            break;
        case DERROTA:
            verificarHoverDerrota(x, y);
            break;
        case RESUMEN_FINAL:
            verificarHoverFinal(x, y);
            break;
        default:
            break;
    }
    
    update();
}

void MainWindow::verificarHoverSound(int x, int y)
{
    hoverSound = btnSound.contains(x, y);
    if (hoverSound) {
        setCursor(Qt::PointingHandCursor);
    }
}

void MainWindow::verificarHoverPausa(int x, int y)
{
    hoverContinuar = btnContinuar.contains(x, y);
    hoverSalirPausa = btnSalirPausa.contains(x, y);
    if (!hoverSound) {
        setCursor((hoverContinuar || hoverSalirPausa) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
}

void MainWindow::verificarHoverVictoria(int x, int y)
{
    hoverAvanzar = btnAvanzar.contains(x, y);
    hoverSalirVictoria = btnSalirVictoria.contains(x, y);
    if (!hoverSound) {
        setCursor((hoverAvanzar || hoverSalirVictoria) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
}

void MainWindow::verificarHoverDerrota(int x, int y)
{
    hoverMenuDerrota = btnMenuDerrota.contains(x, y);
    hoverSalirDerrota = btnSalirDerrota.contains(x, y);
    if (!hoverSound) {
        setCursor((hoverMenuDerrota || hoverSalirDerrota) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
}

void MainWindow::verificarHoverFinal(int x, int y)
{
    hoverMenuFinal = btnMenuFinal.contains(x, y);
    hoverSalirFinal = btnSalirFinal.contains(x, y);
    if (!hoverSound) {
        setCursor((hoverMenuFinal || hoverSalirFinal) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
}

void MainWindow::toggleSonido()
{
    SoundManager::getInstance().toggleSound();
    qDebug() << "[MainWindow] Sonido" << (SoundManager::getInstance().isSoundEnabled() ? "activado" : "desactivado");
    update();
}

void MainWindow::actualizarJuego()
{
    if (estadoJuego != JUGANDO) {
        update();
        return;
    }

    float deltaTime = 0.016f;

    bool arriba = teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_Up);
    bool abajo = teclasPresionadas.contains(Qt::Key_S) || teclasPresionadas.contains(Qt::Key_Down);
    bool izquierda = teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left);
    bool derecha = teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right);

    switch (nivelActual) {
        case 1:
            if (nivel1) nivel1->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
            break;
        case 2:
            if (nivel2) nivel2->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
            break;
        case 3:
            if (nivel3) nivel3->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
            break;
    }

    verificarEstadoNivel();
    update();
}

void MainWindow::verificarEstadoNivel()
{
    bool victoria = false;
    bool derrota = false;
    int puntaje = 0;

    switch (nivelActual) {
        case 1:
            if (nivel1 && !nivel1->esJuegoActivo()) {
                puntaje = nivel1->getPuntuacion();
                victoria = (nivel1->getLetraActualIndex() >= nivel1->getPalabraObjetivo().length());
                derrota = !victoria;
            }
            break;
        case 2:
            if (nivel2 && !nivel2->esJuegoActivo()) {
                puntaje = nivel2->getPuntuacion();
                victoria = (nivel2->getPergaminosEntregados() >= 6);
                derrota = !victoria;
            }
            break;
        case 3:
            if (nivel3 && !nivel3->esJuegoActivo()) {
                puntaje = nivel3->getPuntuacion();
                victoria = (nivel3->getFragmentosEntregados() >= 5);
                derrota = !victoria;
            }
            break;
    }

    if (victoria) {
        switch (nivelActual) {
            case 1: puntajeNivel1 = puntaje; nivel1Completado = true; break;
            case 2: puntajeNivel2 = puntaje; nivel2Completado = true; break;
            case 3: puntajeNivel3 = puntaje; nivel3Completado = true; break;
        }
        
        // Reproducir música de victoria
        SoundManager::getInstance().playVictoryMusic();
        
        if (nivelActual == 3) {
            mostrarResumenFinal();
        } else {
            estadoJuego = VICTORIA;
        }
        qDebug() << "[MainWindow] ¡VICTORIA! Nivel" << nivelActual << "- Puntaje:" << puntaje;
    } else if (derrota) {
        // Reproducir música de derrota
        SoundManager::getInstance().playDefeatMusic();
        
        estadoJuego = DERROTA;
        qDebug() << "[MainWindow] DERROTA en Nivel" << nivelActual;
    }
}

void MainWindow::pausarJuego()
{
    estadoJuego = PAUSADO;
    SoundManager::getInstance().pauseBackgroundMusic();
    qDebug() << "[MainWindow] Juego pausado";
}

void MainWindow::continuarJuego()
{
    estadoJuego = JUGANDO;
    SoundManager::getInstance().resumeBackgroundMusic();
    qDebug() << "[MainWindow] Juego reanudado";
}

void MainWindow::avanzarSiguienteNivel()
{
    int siguienteNivel = nivelActual + 1;
    if (siguienteNivel <= 3) {
        qDebug() << "[MainWindow] Avanzando al Nivel" << siguienteNivel;
        inicializarNivel(siguienteNivel);
    } else {
        mostrarResumenFinal();
    }
}

void MainWindow::mostrarResumenFinal()
{
    estadoJuego = RESUMEN_FINAL;
    SoundManager::getInstance().playVictoryMusic();
    qDebug() << "[MainWindow] Mostrando resumen final";
}

void MainWindow::volverAlMenuPrincipal()
{
    qDebug() << "[MainWindow] Volviendo al menú principal";
    SoundManager::getInstance().playMenuMusic();
    emit volverAlMenu();
    close();
}

void MainWindow::salirDelJuego()
{
    qDebug() << "[MainWindow] Saliendo del juego";
    QApplication::quit();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Dibujar nivel activo
    switch (nivelActual) {
        case 1:
            if (nivel1) nivel1->dibujar(painter);
            break;
        case 2:
            if (nivel2) nivel2->dibujar(painter);
            break;
        case 3:
            if (nivel3) nivel3->dibujar(painter);
            break;
        default:
            painter.fillRect(0, 0, 1024, 768, Qt::black);
            break;
    }
    
    // Dibujar botón de sonido siempre visible (arriba a la derecha)
    dibujarBotonSonido(painter);

    // Dibujar pantallas de estado
    switch (estadoJuego) {
        case PAUSADO:
            dibujarPantallaPausa(painter);
            break;
        case VICTORIA:
            dibujarPantallaVictoria(painter);
            break;
        case DERROTA:
            dibujarPantallaDerrota(painter);
            break;
        case RESUMEN_FINAL:
            dibujarResumenFinal(painter);
            break;
        default:
            break;
    }
}

void MainWindow::dibujarBotonSonido(QPainter &painter)
{
    bool soundEnabled = SoundManager::getInstance().isSoundEnabled();
    QPixmap &sprite = soundEnabled ? spriteSoundOn : spriteSoundOff;
    
    if (!sprite.isNull()) {
        // Efecto hover
        if (hoverSound) {
            painter.setOpacity(1.0f);
        } else {
            painter.setOpacity(0.8f);
        }
        painter.drawPixmap(btnSound, sprite);
        painter.setOpacity(1.0f);
    } else {
        // Fallback si no hay sprite
        painter.setBrush(soundEnabled ? QColor(50, 150, 50) : QColor(150, 50, 50));
        painter.setPen(hoverSound ? Qt::white : Qt::gray);
        painter.drawEllipse(btnSound);
        
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(Qt::white);
        painter.drawText(btnSound, Qt::AlignCenter, soundEnabled ? "ON" : "OFF");
    }
}

void MainWindow::dibujarBotonEstado(QPainter &painter, const QRect &rect, const QString &texto, 
                                     bool hover, const QColor &colorBase)
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    if (hover) {
        gradient.setColorAt(0, colorBase.lighter(140));
        gradient.setColorAt(1, colorBase);
    } else {
        gradient.setColorAt(0, colorBase);
        gradient.setColorAt(1, colorBase.darker(140));
    }
    
    painter.setBrush(gradient);
    painter.setPen(QPen(hover ? colorBase.lighter(180) : colorBase.lighter(120), 2));
    painter.drawRoundedRect(rect, 8, 8);
    
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(rect, Qt::AlignCenter, texto);
}

void MainWindow::dibujarPantallaPausa(QPainter &painter)
{
    // Overlay semitransparente sobre el nivel
    painter.fillRect(0, 0, 1024, 768, QColor(0, 0, 0, 150));
    
    // Usar fondo de pausa si existe
    if (!fondoPausa.isNull()) {
        // Dibujar el marco de opciones centrado pero más pequeño
        QPixmap pausaEscalada = fondoPausa.scaled(600, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int px = (1024 - pausaEscalada.width()) / 2;
        int py = (768 - pausaEscalada.height()) / 2 - 50;
        painter.drawPixmap(px, py, pausaEscalada);
    }
    
    // Texto de pausa
    painter.setFont(QFont("Georgia", 28, QFont::Bold));
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(QRect(0, 250, 1024, 50), Qt::AlignCenter, "JUEGO PAUSADO");
    
    painter.setFont(QFont("Arial", 12));
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(QRect(0, 310, 1024, 25), Qt::AlignCenter, "Presiona ESC para continuar");
    
    // Botones
    dibujarBotonEstado(painter, btnContinuar, "CONTINUAR", hoverContinuar, QColor(50, 120, 50));
    dibujarBotonEstado(painter, btnSalirPausa, "MENÚ", hoverSalirPausa, QColor(120, 80, 60));
}

void MainWindow::dibujarPantallaVictoria(QPainter &painter)
{
    // Fondo de victoria SIN caja ni opacidad - el título "VICTORIA" ya está en la imagen
    if (!fondoVictoria.isNull()) {
        painter.drawPixmap(0, 0, fondoVictoria);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(0, 50, 0));
    }
    
    // Mensaje del nivel (debajo del título de la imagen, aproximadamente a 2/3 de altura)
    QString mensajeNivel;
    switch (nivelActual) {
        case 1: mensajeNivel = "¡Has dominado el Taller de Gutenberg!"; break;
        case 2: mensajeNivel = "¡Has salvado la Biblioteca de Alejandría!"; break;
        case 3: mensajeNivel = "¡Has conquistado la Revolución Digital!"; break;
    }
    
    // Sombra para mejor legibilidad
    painter.setFont(QFont("Georgia", 20));
    painter.setPen(QColor(0, 0, 0, 180));
    painter.drawText(QRect(2, 482, 1024, 40), Qt::AlignCenter, mensajeNivel);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(QRect(0, 480, 1024, 40), Qt::AlignCenter, mensajeNivel);
    
    // Puntaje
    int puntaje = 0;
    switch (nivelActual) {
        case 1: puntaje = nivel1 ? nivel1->getPuntuacion() : puntajeNivel1; break;
        case 2: puntaje = nivel2 ? nivel2->getPuntuacion() : puntajeNivel2; break;
        case 3: puntaje = nivel3 ? nivel3->getPuntuacion() : puntajeNivel3; break;
    }
    painter.setFont(QFont("Georgia", 26, QFont::Bold));
    painter.setPen(QColor(0, 0, 0, 180));
    painter.drawText(QRect(2, 532, 1024, 50), Qt::AlignCenter, "Puntaje: " + QString::number(puntaje));
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(QRect(0, 530, 1024, 50), Qt::AlignCenter, "Puntaje: " + QString::number(puntaje));
    
    // Siguiente nivel info
    if (nivelActual < 3) {
        painter.setFont(QFont("Arial", 14));
        painter.setPen(QColor(0, 0, 0, 150));
        QString siguienteInfo;
        switch (nivelActual) {
            case 1: siguienteInfo = "Siguiente: Biblioteca de Alejandría"; break;
            case 2: siguienteInfo = "Siguiente: Revolución Digital"; break;
        }
        painter.drawText(QRect(2, 592, 1024, 30), Qt::AlignCenter, siguienteInfo);
        painter.setPen(QColor(150, 200, 255));
        painter.drawText(QRect(0, 590, 1024, 30), Qt::AlignCenter, siguienteInfo);
    }
    
    // Botones en esquinas inferiores
    dibujarBotonEstado(painter, btnSalirVictoria, "MENÚ", hoverSalirVictoria, QColor(100, 80, 60));
    QString textoAvanzar = (nivelActual < 3) ? "AVANZAR" : "FINALIZAR";
    dibujarBotonEstado(painter, btnAvanzar, textoAvanzar, hoverAvanzar, QColor(50, 120, 50));
}

void MainWindow::dibujarPantallaDerrota(QPainter &painter)
{
    // Fondo de derrota SIN caja ni opacidad - el título "DERROTA" ya está en la imagen
    if (!fondoDerrota.isNull()) {
        painter.drawPixmap(0, 0, fondoDerrota);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(50, 0, 0));
    }
    
    // Mensaje de derrota con sombra para legibilidad
    QString mensajeDerrota;
    switch (nivelActual) {
        case 1: mensajeDerrota = "El tiempo se agotó en el Taller de Gutenberg"; break;
        case 2: mensajeDerrota = "La Biblioteca de Alejandría ha ardido"; break;
        case 3: mensajeDerrota = "Los virus han corrompido los datos"; break;
    }
    
    painter.setFont(QFont("Georgia", 18));
    painter.setPen(QColor(0, 0, 0, 180));
    painter.drawText(QRect(2, 502, 1024, 35), Qt::AlignCenter, mensajeDerrota);
    painter.setPen(QColor(255, 200, 150));
    painter.drawText(QRect(0, 500, 1024, 35), Qt::AlignCenter, mensajeDerrota);
    
    // Consejo
    painter.setFont(QFont("Arial", 14));
    painter.setPen(QColor(0, 0, 0, 150));
    painter.drawText(QRect(2, 552, 1024, 30), Qt::AlignCenter, "¡No te rindas! Inténtalo de nuevo.");
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(QRect(0, 550, 1024, 30), Qt::AlignCenter, "¡No te rindas! Inténtalo de nuevo.");
    
    // Botones en esquinas inferiores
    dibujarBotonEstado(painter, btnMenuDerrota, "MENÚ", hoverMenuDerrota, QColor(100, 80, 60));
    dibujarBotonEstado(painter, btnSalirDerrota, "SALIR", hoverSalirDerrota, QColor(120, 50, 50));
}

void MainWindow::dibujarResumenFinal(QPainter &painter)
{
    // Fondo de victoria para el resumen final SIN opacidad extra
    if (!fondoVictoria.isNull()) {
        painter.drawPixmap(0, 0, fondoVictoria);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(40, 35, 20));
    }
    
    // Título con sombra
    painter.setFont(QFont("Georgia", 32, QFont::Bold));
    painter.setPen(QColor(0, 0, 0, 180));
    painter.drawText(QRect(2, 152, 1024, 50), Qt::AlignCenter, "¡JUEGO COMPLETADO!");
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(QRect(0, 150, 1024, 50), Qt::AlignCenter, "¡JUEGO COMPLETADO!");
    
    // Subtítulo
    painter.setFont(QFont("Georgia", 16));
    painter.setPen(QColor(200, 180, 100));
    painter.drawText(QRect(0, 200, 1024, 30), Qt::AlignCenter, "Has recorrido la historia del conocimiento");
    
    // Tabla de puntajes
    int tableY = 280;
    int lineHeight = 45;
    
    painter.setFont(QFont("Arial", 16));
    
    // Nivel 1
    painter.setPen(nivel1Completado ? QColor(100, 255, 100) : QColor(150, 150, 150));
    painter.drawText(QRect(300, tableY, 300, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, 
                     "Taller de Gutenberg:");
    painter.drawText(QRect(600, tableY, 150, lineHeight), Qt::AlignRight | Qt::AlignVCenter, 
                     nivel1Completado ? QString::number(puntajeNivel1) : "---");
    tableY += lineHeight;
    
    // Nivel 2
    painter.setPen(nivel2Completado ? QColor(100, 255, 100) : QColor(150, 150, 150));
    painter.drawText(QRect(300, tableY, 300, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, 
                     "Biblioteca de Alejandría:");
    painter.drawText(QRect(600, tableY, 150, lineHeight), Qt::AlignRight | Qt::AlignVCenter, 
                     nivel2Completado ? QString::number(puntajeNivel2) : "---");
    tableY += lineHeight;
    
    // Nivel 3
    painter.setPen(nivel3Completado ? QColor(100, 255, 100) : QColor(150, 150, 150));
    painter.drawText(QRect(300, tableY, 300, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, 
                     "Revolución Digital:");
    painter.drawText(QRect(600, tableY, 150, lineHeight), Qt::AlignRight | Qt::AlignVCenter, 
                     nivel3Completado ? QString::number(puntajeNivel3) : "---");
    tableY += lineHeight + 20;
    
    // Línea separadora
    painter.setPen(QPen(QColor(255, 215, 0), 2));
    painter.drawLine(300, tableY, 750, tableY);
    tableY += 20;
    
    // Puntaje total
    int puntajeTotal = puntajeNivel1 + puntajeNivel2 + puntajeNivel3;
    painter.setFont(QFont("Georgia", 22, QFont::Bold));
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(QRect(300, tableY, 200, 50), Qt::AlignLeft | Qt::AlignVCenter, "TOTAL:");
    painter.drawText(QRect(600, tableY, 150, 50), Qt::AlignRight | Qt::AlignVCenter, 
                     QString::number(puntajeTotal));
    
    // Mensaje final
    painter.setFont(QFont("Georgia", 14));
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(QRect(0, 580, 1024, 30), Qt::AlignCenter, 
                     "¡Felicitaciones! Has preservado el conocimiento a través de las eras.");
    
    // Botones
    dibujarBotonEstado(painter, btnMenuFinal, "MENÚ", hoverMenuFinal, QColor(100, 80, 60));
    dibujarBotonEstado(painter, btnSalirFinal, "SALIR", hoverSalirFinal, QColor(120, 50, 50));
}
