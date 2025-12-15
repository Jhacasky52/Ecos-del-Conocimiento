#ifndef CUBETA_H
#define CUBETA_H

#include "Entidad.h"
#include <QPixmap>
#include <QPainter>

/**
 * Clase Cubeta - Objeto lanzable del Nivel 2
 * 
 * La cubeta se llena en la fuente y se lanza en parábola
 * para apagar los fuegos del piso.
 */
class Cubeta : public Entidad
{


public:
    explicit Cubeta(QObject *parent = nullptr);
    ~Cubeta() override = default;

    // Métodos heredados de Entidad
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // Getters
    float getVelocidadX() const { return velocidadX; }
    float getVelocidadY() const { return velocidadY; }
    bool isActiva() const { return activa; }
    bool isEnVuelo() const { return enVuelo; }

    // Setters
    void setVelocidadX(float vx) { velocidadX = vx; }
    void setVelocidadY(float vy) { velocidadY = vy; }
    void setActiva(bool act) { activa = act; }
    void setEnVuelo(bool vuelo) { enVuelo = vuelo; }

    // Sprite
    void setSprite(const QPixmap &sprite) { spriteCubeta = sprite; }

    // Lanzar cubeta
    void lanzar(float x, float y, bool haciaDerecha);

    // Constantes
    static constexpr float VEL_INICIAL = 400.0f;
    static constexpr float ANGULO = 45.0f;
    static constexpr float RADIO_EFECTO = 160.0f;
    static constexpr float GRAVEDAD = 800.0f;

private:
    float velocidadX;
    float velocidadY;
    bool activa;
    bool enVuelo;

    QPixmap spriteCubeta;
};

#endif // CUBETA_H
