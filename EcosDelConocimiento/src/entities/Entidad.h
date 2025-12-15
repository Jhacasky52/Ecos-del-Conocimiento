#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QPainter>

//
//Clase base abstracta para todas las entidades del juego.
// 
// Esta es una HERENCIA PROPIA (no de Qt) que cumple con el requisito
// del proyecto. Jugador, Automata y Letra heredan de esta clase.
// 
// Proporciona:
// - Posición (posX, posY)
// - Velocidad (velocidadX, velocidadY)
// - Estado activo/inactivo
// - Métodos virtuales puros para actualizar y dibujar

class Entidad
{
public:
    // Constructor y destructor
    Entidad(float x = 0.0f, float y = 0.0f);
    virtual ~Entidad() = default;

    // =========================================================================
    // MÉTODOS VIRTUALES PUROS - Deben ser implementados por las clases hijas
    // =========================================================================
    
    //
    // Actualiza el estado de la entidad cada frame
    // Param: deltaTime Tiempo transcurrido desde el último frame
    
    virtual void actualizar(float deltaTime) = 0;
    
    //
    // Dibuja la entidad en pantalla
    //Param: painter Objeto QPainter para renderizar
    
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
    void setPosX(float x) { posX = x; }
    void setPosY(float y) { posY = y; }
    void setVelocidad(float vx, float vy);
    void setActivo(bool estado) { activo = estado; }
    void setTamanio(int w, int h) { ancho = w; alto = h; }

    // =========================================================================
    // GETTERS ADICIONALES
    // =========================================================================
    int getAncho() const { return ancho; }
    int getAlto() const { return alto; }

    // =========================================================================
    // MÉTODOS DE UTILIDAD
    // =========================================================================
    
    //
    // Calcula la distancia a otra entidad
    //Param: otra Puntero a otra entidad
    //Return: Distancia euclidiana
    
    float distanciaA(const Entidad* otra) const;
    
    //
    // Calcula la distancia a un punto
    // Param: x Coordenada X del punto
    // Param: y Coordenada Y del punto
    // Return: Distancia euclidiana
    
    float distanciaA(float x, float y) const;

protected:
    // Posición en el mundo
    float posX;
    float posY;
    
    // Velocidad actual
    float velocidadX;
    float velocidadY;
    
    // Tamaño de la entidad
    int ancho;
    int alto;
    
    // Estado de la entidad
    bool activo;
};

#endif // ENTIDAD_H
