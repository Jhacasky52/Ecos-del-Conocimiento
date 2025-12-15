#ifndef NODO_H
#define NODO_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Nodo - Punto de regeneración del Nivel 3
 * 
 * Los nodos regeneran la energía del jugador.
 * Tienen un tiempo de recarga después de ser usados.
 */
class Nodo : public Entidad
{
public:
    explicit Nodo(QObject *parent = nullptr);
    ~Nodo() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getRadio() const { return radio; }
    bool isActivo() const { return activo; }
    float getTiempoRecarga() const { return tiempoRecarga; }
    float getPulso() const { return pulso; }

    // Setters
    void setRadio(float r) { radio = r; }
    void setPulso(float p) { pulso = p; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteNodo = sprite; }

    // Usar el nodo
    void usar();

    // Constantes
    static constexpr float TIEMPO_RECARGA = 8.0f;
    static constexpr float ENERGIA = 30.0f;

private:
    float radio;
    float tiempoRecarga;
    float pulso;

    QPixmap spriteNodo;
};

#endif // NODO_H
