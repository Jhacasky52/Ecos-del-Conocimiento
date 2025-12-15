#ifndef FIREWALL_H
#define FIREWALL_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Firewall - Obstáculo del Nivel 3
 * 
 * Los firewalls son barreras que rebotan al jugador.
 * Pueden ser estáticos o móviles.
 */
class Firewall : public Entidad
{


public:
    explicit Firewall(QObject *parent = nullptr);
    ~Firewall() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    bool isMovil() const { return movil; }
    float getVelocidadY() const { return velocidadY; }
    float getLimiteInferior() const { return limiteInferior; }
    float getLimiteSuperior() const { return limiteSuperior; }

    // Setters
    void setMovil(bool m) { movil = m; }
    void setVelocidadY(float vy) { velocidadY = vy; }
    void setLimites(float inferior, float superior) { 
        limiteInferior = inferior; 
        limiteSuperior = superior; 
    }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteFirewall = sprite; }

    // Colisión
    bool colisionaCon(float x, float y, float radio) const;

    // Constantes
    static constexpr float COEFICIENTE_RESTITUCION = 0.7f;

private:
    bool movil;
    float velocidadY;
    float limiteInferior;
    float limiteSuperior;

    QPixmap spriteFirewall;
};

#endif // FIREWALL_H
