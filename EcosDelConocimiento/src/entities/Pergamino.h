#ifndef PERGAMINO_H
#define PERGAMINO_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Pergamino - Objeto coleccionable del Nivel 2
 * 
 * Los pergaminos caen del cielo y deben ser llevados al cofre.
 * Pueden prenderse fuego si tocan el fuego del piso.
 * 
 * Estados:
 * - NORMAL: Sin daño
 * - POCO_FUEGO: Acaba de prenderse (5 segundos)
 * - MUCHO_FUEGO: Casi quemado (5 segundos más)
 * - QUEMADO: Destruido
 */
class Pergamino : public Entidad
{
public:
    enum EstadoPergamino {
        NORMAL,
        POCO_FUEGO,
        MUCHO_FUEGO,
        QUEMADO
    };

    explicit Pergamino(QObject *parent = nullptr);
    ~Pergamino() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelocidadY() const { return velocidadY; }
    bool isEnAire() const { return enAire; }
    bool isRecogido() const { return recogido; }
    EstadoPergamino getEstado() const { return estado; }
    float getTiempoEnFuego() const { return tiempoEnFuego; }
    bool isActivo() const { return activo; }

    // Setters
    void setVelocidadY(float vy) { velocidadY = vy; }
    void setEnAire(bool aire) { enAire = aire; }
    void setRecogido(bool rec) { recogido = rec; }
    void setEstado(EstadoPergamino est) { estado = est; }
    void setTiempoEnFuego(float tiempo) { tiempoEnFuego = tiempo; }

    // Sprites
    void setSprites(const QPixmap &normal, const QPixmap &pocoFuego, const QPixmap &muchoFuego);

    // Acciones
    void prenderFuego();
    void apagarFuego();
    bool estaQuemado() const { return estado == QUEMADO; }

    // Constantes
    static constexpr float VELOCIDAD_CAIDA = 140.0f;
    static constexpr float TIEMPO_POCO_FUEGO = 3.0f;
    static constexpr float TIEMPO_MUCHO_FUEGO = 3.0f;
    static constexpr float TAMANIO = 73.0f;

private:
    float velocidadY;
    bool enAire;
    bool recogido;
    EstadoPergamino estado;
    float tiempoEnFuego;

    // Sprites
    QPixmap spriteNormal;
    QPixmap spritePocoFuego;
    QPixmap spriteMuchoFuego;
};

#endif // PERGAMINO_H
