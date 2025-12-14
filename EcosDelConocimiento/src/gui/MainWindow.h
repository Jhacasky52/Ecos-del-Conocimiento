#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSet>
#include "Nivel1.h"

class QTimer;

/**
 * @brief Ventana principal del juego.
 * 
 * Maneja:
 * - Loop principal del juego (60 FPS)
 * - Entrada de teclado y mouse
 * - Renderizado
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void actualizarJuego();

private:
    Nivel1 *nivel;
    QTimer *timer;
    QSet<int> teclasPresionadas;
    
    void inicializarJuego();
};

#endif // MAINWINDOW_H
