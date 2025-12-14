#include "MainWindow.h"
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), nivel(nullptr), timer(nullptr)
{
    setWindowTitle("Ecos del Conocimiento - Nivel 1");
    setFixedSize(1024, 768);
    setStyleSheet("background-color: black;");
    setFocusPolicy(Qt::StrongFocus);

    inicializarJuego();

    // Configurar timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16); // ~60 FPS

    qDebug() << "[Main] Juego iniciado correctamente";
}

MainWindow::~MainWindow()
{
    if (nivel) delete nivel;
    if (timer) delete timer;
}

void MainWindow::inicializarJuego()
{
    nivel = new Nivel1(this);
    qDebug() << "[MainWindow] Juego inicializado";
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int key = event->key();
    teclasPresionadas.insert(key);

    // Validar que nivel y jugador existan
    if (!nivel || !nivel->getJugador()) {
        return;
    }

    // Tecla R - Recoger/Soltar letras
    if (key == Qt::Key_R) {
        qDebug() << "[MainWindow] Tecla R presionada";
        auto letrasRecogidas = nivel->getJugador()->getLetrasRecogidas();
        if (!letrasRecogidas.isEmpty()) {
            nivel->soltarUltimaLetra();
        } else {
            nivel->intentarRecogerLetra();
        }
        return;
    }

    // Tecla E - Entregar letra en la prensa e interactuar con autómata
    if (key == Qt::Key_E) {
        qDebug() << "[MainWindow] Tecla E presionada";

        // Intentar interactuar con el autómata
        nivel->intentarInteractuarConAutomata();

        // También intentar entregar letra en la prensa
        float jugadorX = nivel->getJugador()->getPosX();
        float jugadorY = nivel->getJugador()->getPosY();
        float distX = jugadorX - 500.0f;
        float distY = jugadorY - 360.0f;
        float distancia = sqrt(distX * distX + distY * distY);
        qDebug() << "[MainWindow] Distancia a prensa:" << distancia;
        if (distancia <= 250.0f) {
            qDebug() << "[MainWindow] ¡En rango! Entregando letra...";
            nivel->verificarLetraEntregada();
        } else {
            qDebug() << "[MainWindow] Muy lejos de la prensa";
        }
        return;
    }

    // Tecla ESC - Salir
    if (key == Qt::Key_Escape) {
        close();
        return;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int key = event->key();
    teclasPresionadas.remove(key);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!nivel) return;

    if (event->button() == Qt::LeftButton) {
        float mouseX = event->position().x();
        float mouseY = event->position().y();
        qDebug() << "[MainWindow] Click en:" << mouseX << "," << mouseY;
        nivel->intentarRecogerLetraConClick(mouseX, mouseY);
    }
}

void MainWindow::actualizarJuego()
{
    if (!nivel) return;

    float deltaTime = 0.016f;

    // Detectar teclas presionadas
    bool arriba = teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_Up);
    bool abajo = teclasPresionadas.contains(Qt::Key_S) || teclasPresionadas.contains(Qt::Key_Down);
    bool izquierda = teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left);
    bool derecha = teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right);

    // Actualizar nivel
    nivel->actualizar(arriba, abajo, izquierda, derecha, deltaTime);

    // Redibujar
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!nivel) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    nivel->dibujar(painter);
}
