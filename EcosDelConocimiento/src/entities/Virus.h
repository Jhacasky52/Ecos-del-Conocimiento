#ifndef VIRUS_H
#define VIRUS_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Virus - Enemigo del Nivel 3
 * 
 * Los virus se mueven con un patrón oscilatorio
 * y dañan al jugador al tocarlo.
 */
class Virus : public Entidad
{


public:
    explicit Virus(QObject *parent = nullptr);
    ~Virus() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelocidadBaseX() const { return velocidadBaseX; }
    float getVelocidadBaseY() const { return velocidadBaseY; }
    float getAmplitud() const { return amplitud; }
    float getFrecuencia() const { return frecuencia; }
    float getFase() const { return fase; }
    float getRadio() const { return radio; }

    // Setters
    void setVelocidadBaseX(float vx) { velocidadBaseX = vx; }
    void setVelocidadBaseY(float vy) { velocidadBaseY = vy; }
    void setAmplitud(float amp) { amplitud = amp; }
    void setFrecuencia(float freq) { frecuencia = freq; }
    void setFase(float f) { fase = f; }
    void setRadio(float r) { radio = r; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteVirus = sprite; }

    // Constantes
    static constexpr float DANIO = 20.0f;

private:
    float velocidadBaseX;
    float velocidadBaseY;
    float amplitud;
    float frecuencia;
    float fase;
    float radio;

    QPixmap spriteVirus;
};

#endif // VIRUS_H
