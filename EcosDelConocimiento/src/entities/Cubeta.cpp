#include "Cubeta.h"
#include <cmath>
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Cubeta::Cubeta(QObject *parent)
    : Entidad(),
      activa(false),
      enVuelo(false)
{
    Q_UNUSED(parent);
    velocidadX = 0.0f;
    velocidadY = 0.0f;
    setTamanio(60, 60);
}

void Cubeta::lanzar(float x, float y, bool haciaDerecha)
{
    posX = x;
    posY = y;
    
    float anguloRad = ANGULO * M_PI / 180.0f;
    velocidadX = VEL_INICIAL * cos(anguloRad) * (haciaDerecha ? 1 : -1);
    velocidadY = -VEL_INICIAL * sin(anguloRad);
    
    activa = true;
    enVuelo = true;
    
    qDebug() << "[Cubeta] Lanzada desde" << x << y << "hacia" << (haciaDerecha ? "derecha" : "izquierda");
}

void Cubeta::actualizar(float deltaTime)
{
    if (!activa || !enVuelo) return;
    
    // Física parabólica
    velocidadY += GRAVEDAD * deltaTime;
    posX += velocidadX * deltaTime;
    posY += velocidadY * deltaTime;
    
    // Desactivar si sale de pantalla o toca el suelo (720)
    if (posY > 720.0f || posX < -100 || posX > 2600) {
        enVuelo = false;
        // activa sigue siendo true para verificar colisiones
    }
}

void Cubeta::dibujar(QPainter &painter)
{
    if (!activa || !enVuelo) return;
    
    if (!spriteCubeta.isNull()) {
        QPixmap escalado = spriteCubeta.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - alto/2),
                          escalado);
    } else {
        // Fallback
        painter.setBrush(QColor(100, 150, 255));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(QPointF(posX, posY), 30, 25);
    }
}
