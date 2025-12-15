#ifndef FRAGMENTO_H
#define FRAGMENTO_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Fragmento - Objeto coleccionable del Nivel 3
 * 
 * Los fragmentos de conocimiento deben ser entregados
 * al terminal para ganar el nivel.
 * Se mueven con un patrón similar a los virus.
 */
class Fragmento : public Entidad
{


public:
    explicit Fragmento(QObject *parent = nullptr);
    ~Fragmento() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    bool isRecogido() const { return recogido; }
    bool isEntregado() const { return entregado; }
    float getOscilacionFase() const { return oscilacionFase; }
    float getVelocidadBaseX() const { return velocidadBaseX; }
    float getVelocidadBaseY() const { return velocidadBaseY; }
    float getAmplitud() const { return amplitud; }
    float getFrecuencia() const { return frecuencia; }

    // Setters
    void setRecogido(bool rec) { recogido = rec; }
    void setEntregado(bool ent) { entregado = ent; }
    void setOscilacionFase(float fase) { oscilacionFase = fase; }
    void setVelocidadBaseX(float vx) { velocidadBaseX = vx; }
    void setVelocidadBaseY(float vy) { velocidadBaseY = vy; }
    void setAmplitud(float amp) { amplitud = amp; }
    void setFrecuencia(float freq) { frecuencia = freq; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteFragmento = sprite; }

private:
    bool recogido;
    bool entregado;
    float oscilacionFase;
    float velocidadBaseX;
    float velocidadBaseY;
    float amplitud;
    float frecuencia;

    QPixmap spriteFragmento;
};

#endif // FRAGMENTO_H
