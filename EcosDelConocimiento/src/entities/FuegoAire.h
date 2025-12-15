#ifndef FUEGOAIRE_H
#define FUEGOAIRE_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase FuegoAire - Fuego cayendo del cielo en el Nivel 2
 * 
 * El fuego cae del cielo y al tocar el piso se esparce
 * creando FuegoPiso.
 */
class FuegoAire : public Entidad
{
public:
    explicit FuegoAire(QObject *parent = nullptr);
    ~FuegoAire() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelocidadY() const { return velocidadY; }
    bool isActivo() const { return activo; }

    // Setters
    void setVelocidadY(float vy) { velocidadY = vy; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteFuego = sprite; }

    // Constantes
    static constexpr float VELOCIDAD_CAIDA = 200.0f;
    static constexpr float TAMANIO = 73.0f;

private:
    float velocidadY;
    QPixmap spriteFuego;
};

#endif // FUEGOAIRE_H
