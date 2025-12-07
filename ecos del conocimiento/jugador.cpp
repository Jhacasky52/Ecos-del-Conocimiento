#include "Jugador.h"
#include "ResourceManager.h"
#include <QDebug>

Jugador::Jugador(QObject *parent)
    : QObject(parent),
    posX(400.0f),
    posY(600.0f),
    velocidad(300.0f),
    direccion(FRENTE),
    anchoMundo(1024.0f),
    altoMundo(768.0f),
    frameAnimacion(0),
    tiempoAnimacion(0.0f),
    estaQuieto(true)
{
    cargarSprites();
    qDebug() << "[Jugador] Creado en posición:" << posX << posY;
}

Jugador::~Jugador()
{
    qDebug() << "[Jugador] Destruido";
}

void Jugador::cargarSprites()
{
    auto &rm = ResourceManager::getInstance();

    spriteFrente = rm.getSprite("nivel1_jugador_frente");
    spriteEspalda = rm.getSprite("nivel1_jugador_espalda");
    spriteIzquierda = rm.getSprite("nivel1_jugador_izquierda");
    spriteDerecha = rm.getSprite("nivel1_jugador_derecha");

    qDebug() << "\n[Jugador] Sprites cargados:";
    qDebug() << "  - Frente:" << spriteFrente.size();
    qDebug() << "  - Espalda:" << spriteEspalda.size();
    qDebug() << "  - Izquierda:" << spriteIzquierda.size();
    qDebug() << "  - Derecha:" << spriteDerecha.size() << "\n";
}

void Jugador::setPosicion(float x, float y)
{
    posX = x;
    posY = y;
}

void Jugador::setLimites(float ancho, float alto)
{
    anchoMundo = ancho;
    altoMundo = alto;
}

void Jugador::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    float dx = 0.0f;
    float dy = 0.0f;

    if (arriba) {
        dy -= velocidad * deltaTime;
        direccion = ESPALDA;
    }
    if (abajo) {
        dy += velocidad * deltaTime;
        direccion = FRENTE;
    }
    if (izquierda) {
        dx -= velocidad * deltaTime;
        direccion = IZQUIERDA;
    }
    if (derecha) {
        dx += velocidad * deltaTime;
        direccion = DERECHA;
    }

    bool moviendo = (dx != 0.0f || dy != 0.0f);
    posX += dx;
    posY += dy;

    restriccionPantalla();

    // Animación
    if (moviendo) {
        tiempoAnimacion += deltaTime;
        estaQuieto = false;

        // Cambiar frame cada 0.15 segundos
        if (tiempoAnimacion >= 0.15f) {
            cambiarFrame();
            tiempoAnimacion = 0.0f;
        }
    } else {
        estaQuieto = true;
        frameAnimacion = 0;
        tiempoAnimacion = 0.0f;
    }
}

void Jugador::cambiarFrame()
{
    // Alternar entre frame 0 y 1 (tienes 2 frames por dirección)
    frameAnimacion = (frameAnimacion == 0) ? 1 : 0;
}

void Jugador::restriccionPantalla()
{
    if (posX < 0) posX = 0;
    if (posX + ANCHO_JUGADOR > anchoMundo) posX = anchoMundo - ANCHO_JUGADOR;
    if (posY < 0) posY = 0;
    if (posY + ALTO_JUGADOR > altoMundo) posY = altoMundo - ALTO_JUGADOR;
}

QPixmap Jugador::getSpriteActual() const
{
    switch (direccion) {
    case FRENTE:    return spriteFrente;
    case ESPALDA:   return spriteEspalda;
    case IZQUIERDA: return spriteIzquierda;
    case DERECHA:   return spriteDerecha;
    default:        return spriteFrente;
    }
}

void Jugador::dibujar(QPainter &painter)
{
    QPixmap spriteDir = getSpriteActual();

    if (spriteDir.isNull()) {
        painter.fillRect(posX, posY, ANCHO_JUGADOR, ALTO_JUGADOR, QColor(255, 0, 0));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(posX + 5, posY + 20, "NO SPRITE");
        return;
    }

    const int numFrames = 2;
    int frameWidth = spriteDir.width() / numFrames;
    int frameHeight = spriteDir.height();

    QRect src(frameAnimacion * frameWidth, 0, frameWidth, frameHeight);

    int drawWidth = frameWidth / 2;
    int drawHeight = frameHeight / 2;

    // Ajuste diferente según el frame
    int offsetX = posX - frameWidth / 4;

    if (direccion == FRENTE) {
        drawWidth = frameWidth /2.4;  // Ajusta este número (5-15)

        if (frameAnimacion == 1) {
            offsetX +=-20;  // Ajusta este número si es necesario
        }
    }

    // Si es el frame 1 (derecho), ajusta su posición X
    if (frameAnimacion == 1) {
        offsetX += 55;  // Mueve 5 píxeles a la derecha
        // Prueba: -10, -5, 0, 5, 10, 15 para encontrar el correcto
    }

    int offsetY = posY - drawHeight;

    QRect dst(offsetX, offsetY, drawWidth, drawHeight);

    painter.drawPixmap(dst, spriteDir, src);
}
