#ifndef NIVEL2_H
#define NIVEL2_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include "Jugador.h"

class Nivel2 : public QObject
{
    Q_OBJECT

public:
    explicit Nivel2(QObject *parent = nullptr);
    ~Nivel2();

    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void dibujar(QPainter &painter);

    Jugador* getJugador() const { return jugador; }

private:
    Jugador* jugador;

    QPixmap fondoOriginal;
    QPixmap fondoEscalado;

    void dibujarFondo(QPainter &painter);
    void dibujarUI(QPainter &painter);
};

#endif // NIVEL2_H
