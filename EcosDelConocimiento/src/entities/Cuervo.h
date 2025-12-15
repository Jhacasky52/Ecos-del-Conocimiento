#ifndef CUERVO_H
#define CUERVO_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Cuervo - Enemigo del Nivel 2
 * 
 * El cuervo es un autómata que roba pergaminos.
 * Estados:
 * - VOLANDO: Buscando un pergamino objetivo
 * - BAJANDO: Descendiendo hacia el pergamino
 * - ROBANDO: Recogiendo el pergamino
 * - ESCAPANDO: Huyendo con el pergamino robado
 */
class Cuervo : public Entidad
{


public:
    enum EstadoCuervo {
        CUERVO_VOLANDO,
        CUERVO_BAJANDO,
        CUERVO_ROBANDO,
        CUERVO_ESCAPANDO
    };

    explicit Cuervo(QObject *parent = nullptr);
    ~Cuervo() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }
    bool isMirandoDerecha() const { return mirandoDerecha; }
    int getFrame() const { return frame; }
    EstadoCuervo getEstado() const { return estado; }
    int getPergaminoObjetivo() const { return pergaminoObjetivo; }
    float getTiempoEspera() const { return tiempoEspera; }
    int getPergaminosRobados() const { return pergaminosRobados; }

    // Setters
    void setVelX(float vx) { velX = vx; }
    void setVelY(float vy) { velY = vy; }
    void setMirandoDerecha(bool derecha) { mirandoDerecha = derecha; }
    void setEstado(EstadoCuervo nuevoEstado) { estado = nuevoEstado; }
    void setPergaminoObjetivo(int idx) { pergaminoObjetivo = idx; }
    void setTiempoEspera(float tiempo) { tiempoEspera = tiempo; }
    void incrementarRobados() { pergaminosRobados++; }

    // Sprites
    void setSprites(const QPixmap &derecha1, const QPixmap &derecha2,
                    const QPixmap &izquierda1, const QPixmap &izquierda2);

    // Actualizar animación
    void actualizarAnimacion(float deltaTime);

    // Constantes
    static constexpr float VELOCIDAD = 180.0f;
    static constexpr float VELOCIDAD_BAJADA = 250.0f;

private:
    float velX, velY;
    bool mirandoDerecha;
    int frame;
    float tiempoFrame;
    EstadoCuervo estado;
    int pergaminoObjetivo;
    float tiempoEspera;
    int pergaminosRobados;

    // Sprites
    QPixmap spriteDerecha1;
    QPixmap spriteDerecha2;
    QPixmap spriteIzquierda1;
    QPixmap spriteIzquierda2;

    static constexpr float TIEMPO_POR_FRAME = 0.2f;
};

#endif // CUERVO_H
