#include "MainWindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <chrono>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), deltaTime(0.016f), nivelActual(1)
{
    // Inicializar ambos niveles
    nivel1 = new Nivel1(this);
    nivel2 = new Nivel2(this);

    // Configurar timer del juego
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    gameTimer->start(16); // ~60 FPS

    lastFrameTime = std::chrono::system_clock::now().time_since_epoch().count();

    setFocus();

    qDebug() << "[MainWindow] Iniciado en Nivel" << nivelActual;
}

MainWindow::~MainWindow()
{
    if (gameTimer) {
        gameTimer->stop();
    }
}

void MainWindow::cambiarNivel(int numeroNivel)
{
    if (numeroNivel == 1 || numeroNivel == 2) {
        nivelActual = numeroNivel;
        qDebug() << "[MainWindow] Cambiado a Nivel" << nivelActual;
        update(); // Redibujar inmediatamente
    } else {
        qWarning() << "[MainWindow] Nivel" << numeroNivel << "no existe";
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Limpiar pantalla
    painter.fillRect(rect(), QColor(20, 40, 80));

    // Dibujar el nivel actual
    if (nivelActual == 1 && nivel1) {
        nivel1->dibujar(painter);
    } else if (nivelActual == 2 && nivel2) {
        nivel2->dibujar(painter);
    }

    // Indicador de nivel en la esquina superior derecha
    painter.setPen(Qt::yellow);
    QFont fontNivel;
    fontNivel.setPointSize(14);
    fontNivel.setBold(true);
    painter.setFont(fontNivel);
    painter.drawText(920, 20, QString("Nivel %1").arg(nivelActual));

    // Instrucciones para cambiar de nivel
    painter.setPen(Qt::cyan);
    QFont fontCambio;
    fontCambio.setPointSize(10);
    painter.setFont(fontCambio);
    painter.drawText(10, 20, "Presiona 1 o 2 para cambiar de nivel");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int key = event->key();

    // Cambio de nivel
    if (key == Qt::Key_1) {
        cambiarNivel(1);
        return;
    } else if (key == Qt::Key_2) {
        cambiarNivel(2);
        return;
    }

    // Movimiento
    if (key == Qt::Key_W) teclaW = true;
    else if (key == Qt::Key_A) teclaA = true;
    else if (key == Qt::Key_S) teclaS = true;
    else if (key == Qt::Key_D) teclaD = true;
    else if (key == Qt::Key_Up) teclaArribaEste = true;
    else if (key == Qt::Key_Left) teclaArribaOeste = true;
    else if (key == Qt::Key_Down) teclaAbajoSur = true;
    else if (key == Qt::Key_Right) teclaAbajoPoloSur = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int key = event->key();

    if (key == Qt::Key_W) teclaW = false;
    else if (key == Qt::Key_A) teclaA = false;
    else if (key == Qt::Key_S) teclaS = false;
    else if (key == Qt::Key_D) teclaD = false;
    else if (key == Qt::Key_Up) teclaArribaEste = false;
    else if (key == Qt::Key_Left) teclaArribaOeste = false;
    else if (key == Qt::Key_Down) teclaAbajoSur = false;
    else if (key == Qt::Key_Right) teclaAbajoPoloSur = false;
}

void MainWindow::actualizarJuego()
{
    calcularDeltaTime();

    // Detectar teclas WASD o Flechas
    bool arriba = teclaW || teclaArribaEste;
    bool abajo = teclaS || teclaAbajoSur;
    bool izquierda = teclaA || teclaArribaOeste;
    bool derecha = teclaD || teclaAbajoPoloSur;

    // Actualizar el nivel actual
    if (nivelActual == 1 && nivel1) {
        nivel1->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
    } else if (nivelActual == 2 && nivel2) {
        nivel2->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
    }

    update(); // Redibujar
}

void MainWindow::calcularDeltaTime()
{
    unsigned long long currentTime = std::chrono::system_clock::now().time_since_epoch().count();
    unsigned long long diff = currentTime - lastFrameTime;
    deltaTime = static_cast<float>(diff) / 1000000000.0f; // Convertir a segundos
    lastFrameTime = currentTime;
}
