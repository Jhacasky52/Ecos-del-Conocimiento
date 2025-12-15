#include "Firewall.h"
#include <cmath>

Firewall::Firewall(QObject *parent)
    : Entidad(),
      movil(false),
      limiteInferior(0.0f),
      limiteSuperior(0.0f)
{
    Q_UNUSED(parent);
    velocidadY = 0.0f;
    setTamanio(30, 200);
}

void Firewall::actualizar(float deltaTime)
{
    if (!movil) return;
    
    posY += velocidadY * deltaTime;
    
    // Rebotar en límites
    if (posY < limiteSuperior || posY > limiteInferior) {
        velocidadY *= -1;
        posY = qBound(limiteSuperior, posY, limiteInferior);
    }
}

bool Firewall::colisionaCon(float x, float y, float radio) const
{
    // Colisión círculo-rectángulo
    float closestX = qBound(posX - ancho/2.0f, x, posX + ancho/2.0f);
    float closestY = qBound(posY - alto/2.0f, y, posY + alto/2.0f);
    
    float dx = x - closestX;
    float dy = y - closestY;
    
    return (dx * dx + dy * dy) < (radio * radio);
}

void Firewall::dibujar(QPainter &painter)
{
    if (!spriteFirewall.isNull()) {
        QPixmap fw = spriteFirewall.scaled(ancho + 20, alto, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2 - 10),
                          static_cast<int>(posY - alto/2),
                          fw);
    } else {
        // Fallback
        painter.setBrush(QColor(255, 150, 0, 180));
        painter.setPen(QPen(QColor(255, 200, 0), 2));
        painter.drawRect(static_cast<int>(posX - ancho/2),
                        static_cast<int>(posY - alto/2),
                        ancho, alto);
    }
}
