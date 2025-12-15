#include "CampoFuerza.h"
#include <cmath>

CampoFuerza::CampoFuerza(QObject *parent)
    : Entidad(),
      radio(100.0f),
      intensidad(50.0f)
{
    Q_UNUSED(parent);
}

bool CampoFuerza::contiene(float x, float y) const
{
    float dx = x - posX;
    float dy = y - posY;
    return (dx * dx + dy * dy) <= (radio * radio);
}

void CampoFuerza::calcularFuerza(float x, float y, float &fx, float &fy) const
{
    float dx = x - posX;
    float dy = y - posY;
    float dist = sqrt(dx * dx + dy * dy);
    
    if (dist < 1.0f || dist > radio) {
        fx = fy = 0.0f;
        return;
    }
    
    // Normalizar dirección
    float nx = dx / dist;
    float ny = dy / dist;
    
    // Fuerza inversamente proporcional a la distancia
    float factor = intensidad * (1.0f - dist / radio);
    
    fx = nx * factor;
    fy = ny * factor;
}

void CampoFuerza::actualizar(float deltaTime)
{
    Q_UNUSED(deltaTime);
    // Los campos de fuerza son estáticos
}

void CampoFuerza::dibujar(QPainter &painter)
{
    // Color según tipo de campo
    QColor color = intensidad < 0 ? QColor(0, 150, 255, 30) : QColor(255, 100, 0, 30);
    
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(posX, posY), radio, radio);
}
