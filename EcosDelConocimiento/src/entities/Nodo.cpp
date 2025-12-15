#include "Nodo.h"
#include <cmath>
#include <QDebug>

Nodo::Nodo(QObject *parent)
    : Entidad(),
      radio(50.0f),
      tiempoRecarga(0.0f),
      pulso(0.0f)
{
    Q_UNUSED(parent);
    activo = true;
    setTamanio(100, 100);
}

void Nodo::usar()
{
    if (activo) {
        activo = false;
        tiempoRecarga = TIEMPO_RECARGA;
        qDebug() << "[Nodo] Usado - regenerando energía";
    }
}

void Nodo::actualizar(float deltaTime)
{
    // Actualizar pulso visual
    pulso += deltaTime * 2.0f;
    
    // Recargar si está inactivo
    if (!activo) {
        tiempoRecarga -= deltaTime;
        if (tiempoRecarga <= 0) {
            activo = true;
            tiempoRecarga = 0;
            qDebug() << "[Nodo] Recargado";
        }
    }
}

void Nodo::dibujar(QPainter &painter)
{
    float escala = 1.0f + sin(pulso) * 0.1f;
    int size = static_cast<int>(radio * 2 * escala);
    
    if (!spriteNodo.isNull() && activo) {
        QPixmap nodo = spriteNodo.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - size/2),
                          static_cast<int>(posY - size/2),
                          nodo);
    } else {
        QColor c = activo ? QColor(0, 255, 255, 150) : QColor(100, 100, 100, 100);
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), radio * escala, radio * escala);
    }
}
