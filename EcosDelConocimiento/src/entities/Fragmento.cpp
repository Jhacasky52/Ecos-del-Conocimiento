#include "Fragmento.h"
#include <cmath>

Fragmento::Fragmento(QObject *parent)
    : Entidad(),
      recogido(false),
      entregado(false),
      oscilacionFase(0.0f),
      amplitud(30.0f),
      frecuencia(1.5f)
{
    Q_UNUSED(parent);
    velocidadBaseX = 0.0f;
    velocidadBaseY = 0.0f;
    setTamanio(50, 50);
}

void Fragmento::actualizar(float deltaTime)
{
    if (recogido || entregado) return;
    
    // Movimiento tipo virus
    oscilacionFase += frecuencia * deltaTime;
    
    float oscX = sin(oscilacionFase) * amplitud * 0.5f;
    float oscY = cos(oscilacionFase * 0.8f) * amplitud * 0.3f;
    
    posX += (velocidadBaseX + oscX) * deltaTime;
    posY += (velocidadBaseY + oscY) * deltaTime;
    
    // Rebotar en bordes
    if (posX < 100 || posX > 924) {
        velocidadBaseX *= -1;
        posX = qBound(100.0f, posX, 924.0f);
    }
    if (posY < 250 || posY > 650) {
        velocidadBaseY *= -1;
        posY = qBound(250.0f, posY, 650.0f);
    }
}

void Fragmento::dibujar(QPainter &painter)
{
    if (entregado) return;
    
    float osc = sin(oscilacionFase) * 5;
    
    if (!spriteFragmento.isNull()) {
        QPixmap frag = spriteFragmento.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - alto/2 + osc),
                          frag);
    } else {
        painter.setBrush(QColor(0, 255, 150, 200));
        painter.setPen(QPen(QColor(100, 255, 200), 2));
        painter.drawEllipse(QPointF(posX, posY + osc), 20, 20);
    }
}
