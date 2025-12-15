#include "FuegoPiso.h"

FuegoPiso::FuegoPiso(QObject *parent)
    : Entidad(),
      tiempoVida(TIEMPO_VIDA)
{
    Q_UNUSED(parent);
    activo = true;
    setTamanio(static_cast<int>(TAMANIO), static_cast<int>(TAMANIO));
}

void FuegoPiso::actualizar(float deltaTime)
{
    if (!activo) return;
    
    tiempoVida -= deltaTime;
    if (tiempoVida <= 0) {
        activo = false;
    }
}

void FuegoPiso::dibujar(QPainter &painter)
{
    if (!activo) return;
    
    // Calcular opacidad basada en tiempo de vida restante
    float opacidad = qMin(1.0f, tiempoVida / TIEMPO_VIDA);
    
    if (!spriteFuego.isNull()) {
        painter.setOpacity(opacidad);
        QPixmap escalado = spriteFuego.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - ancho/2),
                          static_cast<int>(posY - alto/2),
                          escalado);
        painter.setOpacity(1.0f);
    } else {
        // Fallback - llamas en el piso
        QRadialGradient gradient(posX, posY, ancho/2);
        gradient.setColorAt(0, QColor(255, 150, 0, static_cast<int>(200 * opacidad)));
        gradient.setColorAt(0.7, QColor(255, 50, 0, static_cast<int>(150 * opacidad)));
        gradient.setColorAt(1, QColor(100, 0, 0, static_cast<int>(50 * opacidad)));
        
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY - 10), ancho/2, alto/3);
    }
}
