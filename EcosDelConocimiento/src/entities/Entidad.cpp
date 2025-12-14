#include "Entidad.h"
#include <cmath>
#include <QDebug>

Entidad::Entidad(float x, float y)
    : posX(x),
      posY(y),
      velocidadX(0.0f),
      velocidadY(0.0f),
      activo(true)
{
    qDebug() << "[Entidad] Creada en posición:" << posX << "," << posY;
}

void Entidad::setPosicion(float x, float y)
{
    posX = x;
    posY = y;
}

void Entidad::setVelocidad(float vx, float vy)
{
    velocidadX = vx;
    velocidadY = vy;
}

float Entidad::distanciaA(const Entidad* otra) const
{
    if (!otra) return -1.0f;
    return distanciaA(otra->getPosX(), otra->getPosY());
}

float Entidad::distanciaA(float x, float y) const
{
    float dx = posX - x;
    float dy = posY - y;
    return std::sqrt(dx * dx + dy * dy);
}
