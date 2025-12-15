#ifndef FUEGOPISO_H
#define FUEGOPISO_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase FuegoPiso - Fuego esparcido en el piso del Nivel 2
 * 
 * Aparece cuando un FuegoAire toca el suelo.
 * Daña los pergaminos que lo tocan.
 */
class FuegoPiso : public Entidad
{
public:
    explicit FuegoPiso(QObject *parent = nullptr);
    ~FuegoPiso() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getTiempoVida() const { return tiempoVida; }
    bool isActivo() const { return activo; }

    // Setters
    void setTiempoVida(float tiempo) { tiempoVida = tiempo; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteFuego = sprite; }

    // Apagar
    void apagar() { activo = false; }

    // Constantes
    static constexpr float TIEMPO_VIDA = 3.0f;
    static constexpr float TAMANIO = 73.0f;

private:
    float tiempoVida;
    QPixmap spriteFuego;
};

#endif // FUEGOPISO_H
