#include "Letra.h"
#include <QDebug>
#include <ctype.h>
#include <cmath>
#include <QRandomGenerator>

// Constantes físicas
const float Letra::GRAVEDAD = 500.0f;
const float Letra::FRICCION_REBOTE = 0.4f;
const float Letra::VELOCIDAD_MINIMA_REBOTE = 25.0f;
const float Letra::FRICCION_AIRE = 0.99f;

Letra::Letra(QObject *parent)
    : QObject(parent),
      Entidad(0.0f, 0.0f),
      letra('A'),
      anchoLetra(50),
      altoLetra(50),
      recogida(false),
      enReposo(false),        // INICIA CAYENDO (no en reposo)
      pisoDinamico(700.0f)
{
    velocidadX = 0.0f;
    velocidadY = 0.0f;
}

Letra::~Letra()
{
}

void Letra::iniciarCaida(float pisoDestino)
{
    // Método para iniciar la caída al crear la letra
    enReposo = false;
    velocidadX = 0.0f;
    velocidadY = 0.0f;  // Empieza sin velocidad, la gravedad la acelera
    pisoDinamico = pisoDestino;
}

void Letra::setSoltada(float posXSoltada, float posYSoltada)
{
    recogida = false;
    enReposo = false;
    
    posX = posXSoltada;
    posY = posYSoltada;
    
    // =========================================================================
    // MOVIMIENTO PARABÓLICO AL SOLTAR
    // =========================================================================
    float angulo = 35.0f + QRandomGenerator::global()->bounded(25);  // 35-60 grados
    float velocidadInicial = 180.0f + QRandomGenerator::global()->bounded(120);  // 180-300 px/s
    
    // Dirección aleatoria: izquierda (-1) o derecha (+1)
    int direccion = (QRandomGenerator::global()->bounded(2) == 0) ? -1 : 1;
    
    // Convertir a radianes
    float rad = angulo * M_PI / 180.0f;
    
    // Calcular componentes de velocidad (tiro parabólico)
    velocidadX = direccion * velocidadInicial * std::cos(rad);
    velocidadY = -velocidadInicial * std::sin(rad);  // Negativo = hacia arriba
    
    // El piso es donde debe detenerse
    pisoDinamico = posYSoltada + 100.0f;
    
    // Limitar el piso
    if (pisoDinamico > 720.0f) pisoDinamico = 720.0f;
    if (pisoDinamico < 280.0f) pisoDinamico = 280.0f;
    
    qDebug() << "[Letra] Soltada con parábola:" << letra 
             << "Vx:" << (int)velocidadX << "Vy:" << (int)velocidadY;
}

void Letra::setSoltadaSinMovimiento(float posXSoltada, float posYSoltada)
{
    // Método para que el autómata suelte la letra SIN movimiento parabólico
    // La letra simplemente cae hacia abajo desde donde se soltó
    recogida = false;
    enReposo = false;
    
    posX = posXSoltada;
    posY = posYSoltada;
    
    // Sin velocidad horizontal, solo caída vertical suave
    velocidadX = 0.0f;
    velocidadY = 0.0f;  // La gravedad se encargará
    
    // El piso es un poco más abajo de donde se soltó (caída corta)
    pisoDinamico = posYSoltada + 80.0f;  // Solo 80 píxeles de caída
    
    // Limitar el piso
    if (pisoDinamico > 720.0f) pisoDinamico = 720.0f;
    if (pisoDinamico < 280.0f) pisoDinamico = 280.0f;
    
    qDebug() << "[Letra] Soltada SIN parábola (autómata):" << letra 
             << "en X:" << (int)posX << "Y:" << (int)posY
             << "caerá hasta:" << (int)pisoDinamico;
}

void Letra::lanzar(float anguloGrados, float velocidadInicial)
{
    float rad = anguloGrados * M_PI / 180.0f;
    
    velocidadX = velocidadInicial * std::cos(rad);
    velocidadY = -velocidadInicial * std::sin(rad);
    
    recogida = false;
    enReposo = false;
}

void Letra::detener()
{
    velocidadX = 0.0f;
    velocidadY = 0.0f;
    enReposo = true;
    pisoDinamico = posY;
}

void Letra::resetearPosicion()
{
    posY = 150.0f + QRandomGenerator::global()->bounded(20);

    float nuevoX;
    do {
        nuevoX = 100.0f + QRandomGenerator::global()->bounded(820);
    } while (nuevoX >= 280.0f && nuevoX <= 680.0f);

    posX = nuevoX;
    velocidadX = 0.0f;
    velocidadY = 0.0f;

    pisoDinamico = 600.0f + QRandomGenerator::global()->bounded(120);

    enReposo = false;

    qDebug() << "[Letra] Reaparece en Y:" << (int)posY
             << "Piso en:" << (int)pisoDinamico
             << "X:" << (int)posX;
}

void Letra::getLetraCoords(char l, int &col, int &row) const
{
    l = toupper(l);

    if (l >= 'A' && l <= 'E') {
        col = l - 'A'; row = 0;
    } else if (l >= 'F' && l <= 'J') {
        col = l - 'F'; row = 1;
    } else if (l >= 'K' && l <= 'O') {
        col = l - 'K'; row = 2;
    } else if (l >= 'P' && l <= 'T') {
        col = l - 'P'; row = 3;
    } else if (l >= 'U' && l <= 'Y') {
        col = l - 'U'; row = 4;
    } else {
        col = 0; row = 0;
    }
}

QRect Letra::getBoundingBox() const
{
    return QRect(posX - anchoLetra / 2, posY - altoLetra / 2, anchoLetra, altoLetra);
}

bool Letra::colisiona(float x, float y) const
{
    float left = posX - anchoLetra / 2;
    float right = posX + anchoLetra / 2;
    float top = posY - altoLetra / 2;
    float bottom = posY + altoLetra / 2;

    return (x >= left && x <= right && y >= top && y <= bottom);
}

void Letra::actualizar(float deltaTime)
{
    // Si está recogida, no hace nada
    if (recogida) {
        velocidadX = 0.0f;
        velocidadY = 0.0f;
        return;
    }
    
    // Si está en reposo, no aplica física
    if (enReposo) {
        return;
    }

    // =========================================================================
    // FÍSICA: GRAVEDAD Y MOVIMIENTO
    // =========================================================================
    
    velocidadY += GRAVEDAD * deltaTime;
    
    posX += velocidadX * deltaTime;
    posY += velocidadY * deltaTime;
    
    // Fricción del aire en X
    velocidadX *= FRICCION_AIRE;
    if (std::abs(velocidadX) < 1.0f) {
        velocidadX = 0.0f;
    }

    // =========================================================================
    // LÍMITES HORIZONTALES
    // =========================================================================
    if (posX - anchoLetra / 2 < 80.0f) {
        posX = 80.0f + anchoLetra / 2;
        velocidadX = -velocidadX * 0.5f;
    }
    if (posX + anchoLetra / 2 > 940.0f) {
        posX = 940.0f - anchoLetra / 2;
        velocidadX = -velocidadX * 0.5f;
    }
    
    // =========================================================================
    // ZONA DE CAÍDA CENTRAL - Reaparece
    // =========================================================================
    if (posY > 750.0f && posX >= 313.0f && posX <= 648.0f) {
        resetearPosicion();
        return;
    }
    
    // =========================================================================
    // COLISIÓN CON EL PISO
    // =========================================================================
    if (posY >= pisoDinamico) {
        posY = pisoDinamico;
        
        if (std::abs(velocidadY) > VELOCIDAD_MINIMA_REBOTE) {
            velocidadY = -std::abs(velocidadY) * FRICCION_REBOTE;
            velocidadX *= 0.7f;
        } else {
            velocidadY = 0.0f;
            velocidadX = 0.0f;
            enReposo = true;
        }
    }

    // Límite superior
    if (posY < 150.0f) {
        posY = 150.0f;
        if (velocidadY < 0) {
            velocidadY = std::abs(velocidadY) * 0.3f;
        }
    }
    
    // Seguridad: detener si es muy lento
    if (std::abs(velocidadX) < 2.0f && std::abs(velocidadY) < 2.0f && posY >= pisoDinamico - 3.0f) {
        velocidadX = 0.0f;
        velocidadY = 0.0f;
        posY = pisoDinamico;
        enReposo = true;
    }
}

void Letra::dibujar(QPainter &painter)
{
    painter.fillRect(posX - anchoLetra/2, posY - altoLetra/2,
                     anchoLetra, altoLetra, QColor(200, 150, 100));
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(posX - 10, posY + 8, QString(letra));
}

void Letra::dibujar(QPainter &painter, const QPixmap &spriteLetras)
{
    if (spriteLetras.isNull()) {
        dibujar(painter);
        return;
    }

    int col, row;
    getLetraCoords(letra, col, row);

    const int SPRITE_SIZE = 100;
    const int MARGIN = 4;  // Restar 1px de cada lado

    int srcX = col * SPRITE_SIZE + MARGIN;
    int srcY = row * SPRITE_SIZE + MARGIN;
    int srcW = SPRITE_SIZE - MARGIN * 2;  // 98
    int srcH = SPRITE_SIZE - MARGIN * 2;  // 98

    QRect src(srcX, srcY, srcW, srcH);
    QRect dst(posX - anchoLetra / 2, posY - altoLetra / 2,
              anchoLetra, altoLetra);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(dst, spriteLetras, src);

    // QUITADO: Ya no dibujar el cuadro amarillo cuando está recogida
    // if (recogida) {
    //     painter.setPen(QPen(Qt::yellow, 2));
    //     painter.drawRect(dst);
    // }
}
