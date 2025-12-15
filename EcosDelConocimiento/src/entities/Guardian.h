#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Guardian - Enemigo principal del Nivel 3
 * 
 * El guardián persigue al jugador y causa daño al tocarlo.
 * Es más grande y peligroso que los virus.
 */
class Guardian : public Entidad
{
public:
    explicit Guardian(QObject *parent = nullptr);
    ~Guardian() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }
    bool isActivo() const { return activo; }
    float getPulso() const { return pulso; }

    // Setters
    void setVelX(float vx) { velX = vx; }
    void setVelY(float vy) { velY = vy; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteGuardian = sprite; }

    // Perseguir objetivo
    void perseguir(float targetX, float targetY, float deltaTime);

    // Constantes
    static constexpr float VELOCIDAD = 180.0f;
    static constexpr float DANIO = 25.0f;
    static constexpr float RADIO = 40.0f;

private:
    float velX;
    float velY;
    float pulso;

    QPixmap spriteGuardian;
};

#endif // GUARDIAN_H
