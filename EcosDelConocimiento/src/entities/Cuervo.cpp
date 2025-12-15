#include "Cuervo.h"
#include <QDebug>

Cuervo::Cuervo(QObject *parent)
    : Entidad(),
      mirandoDerecha(true),
      frame(0),
      tiempoFrame(0.0f),
      estado(CUERVO_VOLANDO),
      pergaminoObjetivo(-1),
      tiempoEspera(3.0f),
      pergaminosRobados(0)
{
    Q_UNUSED(parent);
    velX = 0.0f;
    velY = 0.0f;
    setPosicion(800.0f, 200.0f);
    setTamanio(150, 100);
    qDebug() << "[Cuervo] Creado";
}

void Cuervo::setSprites(const QPixmap &derecha1, const QPixmap &derecha2,
                        const QPixmap &izquierda1, const QPixmap &izquierda2)
{
    spriteDerecha1 = derecha1;
    spriteDerecha2 = derecha2;
    spriteIzquierda1 = izquierda1;
    spriteIzquierda2 = izquierda2;
}

void Cuervo::actualizarAnimacion(float deltaTime)
{
    tiempoFrame += deltaTime;
    if (tiempoFrame >= TIEMPO_POR_FRAME) {
        tiempoFrame = 0.0f;
        frame = (frame + 1) % 2;
    }
}

void Cuervo::actualizar(float deltaTime)
{
    // La lógica principal se maneja en Nivel2
    actualizarAnimacion(deltaTime);
    
    // Actualizar posición según velocidad
    posX += velX * deltaTime;
    posY += velY * deltaTime;
}

void Cuervo::dibujar(QPainter &painter)
{
    QPixmap sprite;
    if (mirandoDerecha) {
        sprite = (frame == 0) ? spriteDerecha1 : spriteDerecha2;
    } else {
        sprite = (frame == 0) ? spriteIzquierda1 : spriteIzquierda2;
    }

    if (!sprite.isNull()) {
        QPixmap escalado = sprite.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - alto/2),
                          escalado);
    } else {
        // Fallback
        painter.setBrush(QColor(30, 30, 30));
        painter.drawEllipse(QPointF(posX, posY), 80, 50);
    }
}
