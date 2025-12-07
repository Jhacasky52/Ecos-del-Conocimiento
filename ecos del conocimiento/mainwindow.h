#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include "Nivel1.h"
#include "Nivel2.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Método para cambiar de nivel
    void cambiarNivel(int numeroNivel);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarJuego();

private:
    // Niveles
    Nivel1 *nivel1;
    Nivel2 *nivel2;
    int nivelActual; // 1 o 2

    QTimer *gameTimer;

    // Teclas presionadas
    bool teclaW = false;
    bool teclaA = false;
    bool teclaS = false;
    bool teclaD = false;
    bool teclaArribaEste = false;
    bool teclaArribaOeste = false;
    bool teclaAbajoSur = false;
    bool teclaAbajoPoloSur = false;

    float deltaTime;
    unsigned long long lastFrameTime;

    void calcularDeltaTime();
};

#endif // MAINWINDOW_H
