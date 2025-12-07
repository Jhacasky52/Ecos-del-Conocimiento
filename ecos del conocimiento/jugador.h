#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QPainter>
#include <QPixmap>

class Jugador : public QObject
{
    Q_OBJECT

public:
    enum Direccion {
        FRENTE,
        ESPALDA,
        IZQUIERDA,
        DERECHA
    };

    explicit Jugador(QObject *parent = nullptr);
    ~Jugador();

    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    Direccion getDireccion() const { return direccion; }

    void setPosicion(float x, float y);
    void setVelocidad(float vel) { velocidad = vel; }

    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void dibujar(QPainter &painter);

    void setLimites(float ancho, float alto);
    void cargarSprites();

private:
    // Posición y movimiento
    float posX;
    float posY;
    float velocidad;
    Direccion direccion;
    float anchoMundo;
    float altoMundo;

    // Sprites (uno por dirección)
    QPixmap spriteFrente;
    QPixmap spriteEspalda;
    QPixmap spriteIzquierda;
    QPixmap spriteDerecha;

    // Animación
    int frameAnimacion;
    float tiempoAnimacion;
    bool estaQuieto;

    static const int ANCHO_JUGADOR = 64;
    static const int ALTO_JUGADOR = 64;

    // Métodos privados
    void restriccionPantalla();
    QPixmap getSpriteActual() const;
    void cambiarFrame();
};

#endif // JUGADOR_H
