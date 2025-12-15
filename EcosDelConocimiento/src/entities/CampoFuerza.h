#ifndef CAMPOFUERZA_H
#define CAMPOFUERZA_H

#include "Entidad.h"
#include <QPainter>

/**
 * Clase CampoFuerza - Campo de atracción/repulsión del Nivel 3
 * 
 * Los campos de fuerza afectan el movimiento del jugador.
 * - Intensidad positiva = repulsión
 * - Intensidad negativa = atracción
 */
class CampoFuerza : public Entidad
{


public:
    explicit CampoFuerza(QObject *parent = nullptr);
    ~CampoFuerza() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getRadio() const { return radio; }
    float getIntensidad() const { return intensidad; }

    // Setters
    void setRadio(float r) { radio = r; }
    void setIntensidad(float i) { intensidad = i; }

    // Verificar si un punto está dentro del campo
    bool contiene(float x, float y) const;

    // Calcular fuerza aplicada a un punto
    void calcularFuerza(float x, float y, float &fx, float &fy) const;

private:
    float radio;
    float intensidad;
};

#endif // CAMPOFUERZA_H
