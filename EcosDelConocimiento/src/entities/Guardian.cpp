#include "Guardian.h"
#include <cmath>

Guardian::Guardian(QObject *parent)
    : Entidad(),
      pulso(0.0f)
{
    Q_UNUSED(parent);
    velX = 0.0f;
    velY = 0.0f;
    activo = true;
    setPosicion(100.0f, 400.0f);
    setTamanio(80, 80);
}

void Guardian::perseguir(float targetX, float targetY, float deltaTime)
{
    if (!activo) return;
    
    float dx = targetX - posX;
    float dy = targetY - posY;
    float dist = sqrt(dx * dx + dy * dy);
    
    if (dist > 1.0f) {
        velX = (dx / dist) * VELOCIDAD;
        velY = (dy / dist) * VELOCIDAD;
        
        posX += velX * deltaTime;
        posY += velY * deltaTime;
    }
    
    // Mantener dentro de límites
    posX = qBound(50.0f, posX, 974.0f);
    posY = qBound(100.0f, posY, 700.0f);
}

void Guardian::actualizar(float deltaTime)
{
    pulso += deltaTime * 2.0f;
}

void Guardian::dibujar(QPainter &painter)
{
    if (!activo) return;
    
    float escala = 1.0f + sin(pulso) * 0.15f;
    int size = static_cast<int>(80 * escala);
    
    if (!spriteGuardian.isNull()) {
        QPixmap g = spriteGuardian.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(static_cast<int>(posX - size/2),
                          static_cast<int>(posY - size/2),
                          g);
    } else {
        // Fallback - guardián rojo amenazante
        painter.setBrush(QColor(255, 50, 50, 180));
        painter.setPen(QPen(QColor(255, 100, 100), 3));
        painter.drawEllipse(QPointF(posX, posY), 35 * escala, 35 * escala);
        
        // Ojos
        painter.setBrush(Qt::white);
        painter.drawEllipse(QPointF(posX - 10, posY - 5), 6, 6);
        painter.drawEllipse(QPointF(posX + 10, posY - 5), 6, 6);
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPointF(posX - 10, posY - 5), 3, 3);
        painter.drawEllipse(QPointF(posX + 10, posY - 5), 3, 3);
    }
    
    // Texto de advertencia
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(static_cast<int>(posX - 30),
                    static_cast<int>(posY - 50),
                    "GUARDIAN");
}
