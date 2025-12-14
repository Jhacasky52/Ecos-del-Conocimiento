#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QVector>
#include "Entidad.h"  // Herencia propia

class Letra;  // Forward declaration

/**
 * @brief Clase que representa al jugador del juego.
 * 
 * Hereda de:
 * - QObject: Para señales/slots de Qt
 * - Entidad: Herencia propia (requisito del proyecto)
 */
class Jugador : public QObject, public Entidad
{
    Q_OBJECT

public:
    enum Direccion {
        FRENTE,
        ESPALDA,
        IZQUIERDA,
        DERECHA
    };

    explicit Jugador(QObject *parent = nullptr);
    ~Jugador() override;

    // =========================================================================
    // IMPLEMENTACIÓN DE MÉTODOS VIRTUALES DE ENTIDAD
    // =========================================================================
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // =========================================================================
    // MÉTODOS ESPECÍFICOS DEL JUGADOR
    // =========================================================================
    
    /**
     * @brief Actualiza el jugador con entrada del teclado
     */
    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    
    Direccion getDireccion() const { return direccion; }
    void setVelocidadMovimiento(float vel) { velocidadMovimiento = vel; }
    void setLimites(float ancho, float alto);
    void cargarSprites();

    // Sistema de letras recogidas
    QVector<Letra*> getLetrasRecogidas() const { return letrasRecogidas; }
    void agregarLetra(Letra *letra) { letrasRecogidas.append(letra); }
    void soltarLetra(Letra *letra) { letrasRecogidas.removeAll(letra); }

private:
    // Movimiento
    float velocidadMovimiento;
    Direccion direccion;
    float anchoMundo;
    float altoMundo;

    // Sprites (uno por dirección)
    QPixmap spriteFrente;
    QPixmap spriteEspalda;
    QPixmap spriteIzquierda;
    QPixmap spriteDerecha;

    // Animación
    int frameAnimacion;
    float tiempoAnimacion;
    bool estaQuieto;

    // Letras recogidas
    QVector<Letra*> letrasRecogidas;

    static const int ANCHO_JUGADOR = 64;
    static const int ALTO_JUGADOR = 64;

    // Métodos privados
    void restriccionPantalla();
    QPixmap getSpriteActual() const;
    void cambiarFrame();
};

#endif // JUGADOR_H
