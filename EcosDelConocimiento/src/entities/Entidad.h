#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QPainter>

/**
 * @brief Clase base abstracta para todas las entidades del juego.
 * 
 * Esta es una HERENCIA PROPIA (no de Qt) que cumple con el requisito
 * del proyecto. Jugador, Automata y Letra heredan de esta clase.
 * 
 * Proporciona:
 * - Posición (posX, posY)
 * - Velocidad (velocidadX, velocidadY)
 * - Estado activo/inactivo
 * - Métodos virtuales puros para actualizar y dibujar
 */
class Entidad
{
public:
    // Constructor y destructor
    Entidad(float x = 0.0f, float y = 0.0f);
    virtual ~Entidad() = default;

    // =========================================================================
    // MÉTODOS VIRTUALES PUROS - Deben ser implementados por las clases hijas
    // =========================================================================
    
    /**
     * @brief Actualiza el estado de la entidad cada frame
     * @param deltaTime Tiempo transcurrido desde el último frame
     */
    virtual void actualizar(float deltaTime) = 0;
    
    /**
     * @brief Dibuja la entidad en pantalla
     * @param painter Objeto QPainter para renderizar
     */
    virtual void dibujar(QPainter &painter) = 0;

    // =========================================================================
    // GETTERS
    // =========================================================================
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getVelocidadX() const { return velocidadX; }
    float getVelocidadY() const { return velocidadY; }
    bool estaActivo() const { return activo; }

    // =========================================================================
    // SETTERS
    // =========================================================================
    virtual void setPosicion(float x, float y);
    void setVelocidad(float vx, float vy);
    void setActivo(bool estado) { activo = estado; }

    // =========================================================================
    // MÉTODOS DE UTILIDAD
    // =========================================================================
    
    /**
     * @brief Calcula la distancia a otra entidad
     * @param otra Puntero a otra entidad
     * @return Distancia euclidiana
     */
    float distanciaA(const Entidad* otra) const;
    
    /**
     * @brief Calcula la distancia a un punto
     * @param x Coordenada X del punto
     * @param y Coordenada Y del punto
     * @return Distancia euclidiana
     */
    float distanciaA(float x, float y) const;

protected:
    // Posición en el mundo
    float posX;
    float posY;
    
    // Velocidad actual
    float velocidadX;
    float velocidadY;
    
    // Estado de la entidad
    bool activo;
};

#endif // ENTIDAD_H
