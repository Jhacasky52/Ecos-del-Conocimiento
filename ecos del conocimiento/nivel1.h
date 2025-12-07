#ifndef NIVEL1_H
#define NIVEL1_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include "Jugador.h"

class Nivel1 : public QObject
{
    Q_OBJECT

public:
    explicit Nivel1(QObject *parent = nullptr);
    ~Nivel1();

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

#endif // NIVEL1_H
