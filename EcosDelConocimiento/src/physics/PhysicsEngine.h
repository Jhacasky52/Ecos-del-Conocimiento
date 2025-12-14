#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <cmath>
#include <QDebug>

/**
 * @brief Motor de física del juego.
 * 
 * Implementa los modelos físicos requeridos por el proyecto:
 * 
 * 1. CAÍDA LIBRE CON GRAVEDAD
 *    - y(t) = y0 + v0*t + 0.5*g*t²
 *    - v(t) = v0 + g*t
 * 
 * 2. REBOTE CON FRICCIÓN
 *    - v_después = -v_antes * coef_restitución
 *    - Pérdida de energía en cada rebote
 * 
 * 3. MOVIMIENTO PARABÓLICO (Tiro oblicuo)
 *    - x(t) = x0 + v0x*t
 *    - y(t) = y0 + v0y*t - 0.5*g*t²
 *    - v0x = v0 * cos(θ)
 *    - v0y = v0 * sin(θ)
 */
class PhysicsEngine
{
public:
    // Constantes físicas
    static constexpr float GRAVEDAD = 600.0f;           // píxeles/s²
    static constexpr float COEF_RESTITUCION = 0.55f;    // Factor de rebote
    static constexpr float FRICCION_AIRE = 0.98f;       // Resistencia del aire
    static constexpr float VELOCIDAD_MIN_REBOTE = 20.0f; // Umbral para detener

    /**
     * @brief Aplica gravedad a una velocidad vertical
     * @param velocidadY Velocidad actual en Y
     * @param deltaTime Tiempo transcurrido
     * @return Nueva velocidad en Y
     */
    static float aplicarGravedad(float velocidadY, float deltaTime) {
        return velocidadY + GRAVEDAD * deltaTime;
    }

    /**
     * @brief Calcula la nueva posición con velocidad actual
     * @param posicion Posición actual
     * @param velocidad Velocidad actual
     * @param deltaTime Tiempo transcurrido
     * @return Nueva posición
     */
    static float actualizarPosicion(float posicion, float velocidad, float deltaTime) {
        return posicion + velocidad * deltaTime;
    }

    /**
     * @brief Calcula el rebote contra una superficie
     * @param velocidad Velocidad antes del impacto
     * @param coefRestitucion Coeficiente de restitución (0-1)
     * @return Velocidad después del rebote
     */
    static float calcularRebote(float velocidad, float coefRestitucion = COEF_RESTITUCION) {
        float nuevaVelocidad = -velocidad * coefRestitucion;
        
        // Si la velocidad es muy pequeña, detener
        if (std::abs(nuevaVelocidad) < VELOCIDAD_MIN_REBOTE) {
            return 0.0f;
        }
        return nuevaVelocidad;
    }

    /**
     * @brief Aplica fricción del aire a una velocidad
     * @param velocidad Velocidad actual
     * @param friccion Coeficiente de fricción
     * @return Velocidad con fricción aplicada
     */
    static float aplicarFriccion(float velocidad, float friccion = FRICCION_AIRE) {
        float nuevaVelocidad = velocidad * friccion;
        if (std::abs(nuevaVelocidad) < 0.1f) {
            return 0.0f;
        }
        return nuevaVelocidad;
    }

    // =========================================================================
    // MOVIMIENTO PARABÓLICO
    // =========================================================================

    /**
     * @brief Calcula la velocidad inicial X para un tiro oblicuo
     * @param velocidadInicial Velocidad inicial total
     * @param anguloGrados Ángulo de lanzamiento en grados
     * @return Componente X de la velocidad
     */
    static float calcularVelocidadInicialX(float velocidadInicial, float anguloGrados) {
        float rad = anguloGrados * M_PI / 180.0f;
        return velocidadInicial * std::cos(rad);
    }

    /**
     * @brief Calcula la velocidad inicial Y para un tiro oblicuo
     * @param velocidadInicial Velocidad inicial total
     * @param anguloGrados Ángulo de lanzamiento en grados
     * @return Componente Y de la velocidad (negativa = hacia arriba)
     */
    static float calcularVelocidadInicialY(float velocidadInicial, float anguloGrados) {
        float rad = anguloGrados * M_PI / 180.0f;
        return -velocidadInicial * std::sin(rad);  // Negativo porque Y crece hacia abajo
    }

    /**
     * @brief Calcula la altura máxima de un tiro parabólico
     * @param velocidadInicialY Velocidad inicial en Y
     * @param gravedad Aceleración gravitatoria
     * @return Altura máxima alcanzada
     */
    static float calcularAlturaMaxima(float velocidadInicialY, float gravedad = GRAVEDAD) {
        // h_max = v0y² / (2*g)
        return (velocidadInicialY * velocidadInicialY) / (2.0f * gravedad);
    }

    /**
     * @brief Calcula el alcance horizontal de un tiro parabólico
     * @param velocidadInicial Velocidad inicial total
     * @param anguloGrados Ángulo de lanzamiento
     * @param gravedad Aceleración gravitatoria
     * @return Alcance horizontal
     */
    static float calcularAlcance(float velocidadInicial, float anguloGrados, float gravedad = GRAVEDAD) {
        // R = v0² * sin(2θ) / g
        float rad = anguloGrados * M_PI / 180.0f;
        return (velocidadInicial * velocidadInicial * std::sin(2.0f * rad)) / gravedad;
    }

    /**
     * @brief Calcula el tiempo de vuelo de un tiro parabólico
     * @param velocidadInicialY Velocidad inicial en Y
     * @param gravedad Aceleración gravitatoria
     * @return Tiempo total de vuelo
     */
    static float calcularTiempoVuelo(float velocidadInicialY, float gravedad = GRAVEDAD) {
        // t = 2 * v0y / g
        return (2.0f * std::abs(velocidadInicialY)) / gravedad;
    }

    // =========================================================================
    // COLISIONES
    // =========================================================================

    /**
     * @brief Detecta colisión entre dos rectángulos (AABB)
     * @return true si hay colisión
     */
    static bool colisionAABB(float x1, float y1, float w1, float h1,
                             float x2, float y2, float w2, float h2) {
        return (x1 < x2 + w2 &&
                x1 + w1 > x2 &&
                y1 < y2 + h2 &&
                y1 + h1 > y2);
    }

    /**
     * @brief Calcula la distancia entre dos puntos
     */
    static float distancia(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    /**
     * @brief Detecta colisión circular
     * @return true si hay colisión
     */
    static bool colisionCircular(float x1, float y1, float r1,
                                  float x2, float y2, float r2) {
        return distancia(x1, y1, x2, y2) < (r1 + r2);
    }

private:
    PhysicsEngine() = default;  // Clase de utilidades estáticas
};

#endif // PHYSICSENGINE_H
