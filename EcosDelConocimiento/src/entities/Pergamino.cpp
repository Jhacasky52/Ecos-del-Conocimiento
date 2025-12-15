#include "Pergamino.h"
#include <QDebug>

Pergamino::Pergamino(QObject *parent)
    : Entidad(),
      velocidadY(VELOCIDAD_CAIDA),
      enAire(true),
      recogido(false),
      estado(NORMAL),
      tiempoEnFuego(0.0f)
{
    Q_UNUSED(parent);
    activo = true;
    setTamanio(static_cast<int>(TAMANIO), static_cast<int>(TAMANIO));
}

void Pergamino::setSprites(const QPixmap &normal, const QPixmap &pocoFuego, const QPixmap &muchoFuego)
{
    spriteNormal = normal;
    spritePocoFuego = pocoFuego;
    spriteMuchoFuego = muchoFuego;
}

void Pergamino::prenderFuego()
{
    if (estado == NORMAL) {
        estado = POCO_FUEGO;
        tiempoEnFuego = 0.0f;
        qDebug() << "[Pergamino] Se ha prendido fuego!";
    }
}

void Pergamino::apagarFuego()
{
    if (estado == POCO_FUEGO || estado == MUCHO_FUEGO) {
        estado = NORMAL;
        tiempoEnFuego = 0.0f;
        qDebug() << "[Pergamino] Fuego apagado!";
    }
}

void Pergamino::actualizar(float deltaTime)
{
    if (!activo) return;

    // Actualizar caída si está en el aire
    if (enAire && !recogido) {
        posY += velocidadY * deltaTime;
    }

    // Actualizar tiempo de fuego
    if (estado == POCO_FUEGO) {
        tiempoEnFuego += deltaTime;
        if (tiempoEnFuego >= TIEMPO_POCO_FUEGO) {
            estado = MUCHO_FUEGO;
            tiempoEnFuego = 0.0f;
        }
    } else if (estado == MUCHO_FUEGO) {
        tiempoEnFuego += deltaTime;
        if (tiempoEnFuego >= TIEMPO_MUCHO_FUEGO) {
            estado = QUEMADO;
            activo = false;
            qDebug() << "[Pergamino] Se ha quemado completamente!";
        }
    }
}

void Pergamino::dibujar(QPainter &painter)
{
    if (!activo) return;

    QPixmap sprite;
    switch (estado) {
        case POCO_FUEGO:
            sprite = spritePocoFuego;
            break;
        case MUCHO_FUEGO:
            sprite = spriteMuchoFuego;
            break;
        default:
            sprite = spriteNormal;
            break;
    }

    if (!sprite.isNull()) {
        QPixmap escalado = sprite.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - ancho/2),
                          escalado);
    } else {
        // Fallback según estado
        QColor color;
        switch (estado) {
            case POCO_FUEGO: color = QColor(255, 200, 100); break;
            case MUCHO_FUEGO: color = QColor(255, 100, 50); break;
            default: color = QColor(210, 180, 140); break;
        }
        painter.setBrush(color);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(static_cast<int>(posX - ancho/2),
                        static_cast<int>(posY - alto/2),
                        ancho, alto);
    }
}
