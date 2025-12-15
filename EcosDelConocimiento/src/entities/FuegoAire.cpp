#include "FuegoAire.h"

FuegoAire::FuegoAire(QObject *parent)
    : Entidad()
{
    Q_UNUSED(parent);
    velocidadY = VELOCIDAD_CAIDA;
    activo = true;
    setTamanio(static_cast<int>(TAMANIO), static_cast<int>(TAMANIO));
}

void FuegoAire::actualizar(float deltaTime)
{
    if (!activo) return;
    posY += velocidadY * deltaTime;
}

void FuegoAire::dibujar(QPainter &painter)
{
    if (!activo) return;
    
    if (!spriteFuego.isNull()) {
        QPixmap escalado = spriteFuego.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - alto/2),
                          escalado);
    } else {
        // Fallback - bola de fuego naranja
        QRadialGradient gradient(posX, posY, ancho/2);
        gradient.setColorAt(0, QColor(255, 200, 50));
        gradient.setColorAt(0.5, QColor(255, 100, 0));
        gradient.setColorAt(1, QColor(200, 50, 0, 150));
        
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), ancho/2, alto/2);
    }
}
