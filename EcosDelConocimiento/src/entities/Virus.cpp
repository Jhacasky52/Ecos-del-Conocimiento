#include "Virus.h"
#include <cmath>

Virus::Virus(QObject *parent)
    : Entidad(),
      amplitud(30.0f),
      frecuencia(1.5f),
      fase(0.0f),
      radio(35.0f)
{
    Q_UNUSED(parent);
    velocidadBaseX = 0.0f;
    velocidadBaseY = 0.0f;
    setTamanio(70, 70);
}

void Virus::actualizar(float deltaTime)
{
    // Movimiento oscilatorio
    fase += frecuencia * deltaTime;
    
    float oscX = sin(fase) * amplitud;
    float oscY = cos(fase * 0.7f) * amplitud * 0.5f;
    
    posX += (velocidadBaseX + oscX) * deltaTime;
    posY += (velocidadBaseY + oscY) * deltaTime;
    
    // Rebotar en los bordes
    if (posX < 50 || posX > 974) {
        velocidadBaseX *= -1;
        posX = qBound(50.0f, posX, 974.0f);
    }
    if (posY < 100 || posY > 700) {
        velocidadBaseY *= -1;
        posY = qBound(100.0f, posY, 700.0f);
    }
}

void Virus::dibujar(QPainter &painter)
{
    int size = static_cast<int>(radio * 2);
    
    if (!spriteVirus.isNull()) {
        QPixmap vir = spriteVirus.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - size/2),
                          static_cast<int>(posY - size/2),
                          vir);
    } else {
        // Fallback - virus rojo
        painter.setBrush(QColor(255, 50, 50, 200));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), radio, radio);
    }
}
