#ifndef DIFFICULTYMANAGER_H
#define DIFFICULTYMANAGER_H

#include <QDebug>
#include <QString>

/**
 * @brief Gestor de dificultad del juego (Singleton).
 * 
 * Implementa el sistema de dificultad progresiva requerido por el proyecto.
 * Controla:
 * - Tiempo límite por nivel
 * - Velocidad del jugador
 * - Número de letras
 * - Velocidad del autómata
 * - Progresión automática de dificultad
 */

enum class Dificultad {
    FACIL,
    NORMAL,
    DIFICIL
};

class DifficultyManager {
public:
    /**
     * @brief Obtiene la instancia única del DifficultyManager
     */
    static DifficultyManager& getInstance() {
        static DifficultyManager instance;
        return instance;
    }
    
    // =========================================================================
    // GETTERS DE PARÁMETROS DE JUEGO
    // =========================================================================
    
    /**
     * @brief Obtiene el tiempo límite según la dificultad
     * @return Tiempo en segundos
     */
    float getTiempoLimite() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 90.0f;
            case Dificultad::NORMAL:  return 60.0f;
            case Dificultad::DIFICIL: return 40.0f;
            default: return 60.0f;
        }
    }
    
    /**
     * @brief Obtiene la velocidad del jugador según la dificultad
     * @return Velocidad en píxeles/segundo
     */
    float getVelocidadJugador() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 350.0f;
            case Dificultad::NORMAL:  return 300.0f;
            case Dificultad::DIFICIL: return 250.0f;
            default: return 300.0f;
        }
    }
    
    /**
     * @brief Obtiene el número de letras según la dificultad
     * @return Cantidad de letras en el nivel
     */
    int getNumeroLetras() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 25;
            case Dificultad::NORMAL:  return 25;
            case Dificultad::DIFICIL: return 25;
            default: return 25;
        }
    }
    
    /**
     * @brief Obtiene la velocidad del autómata según la dificultad
     * @return Velocidad en píxeles/segundo
     */
    float getVelocidadAutomata() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 150.0f;
            case Dificultad::NORMAL:  return 250.0f;
            case Dificultad::DIFICIL: return 350.0f;
            default: return 250.0f;
        }
    }
    
    /**
     * @brief Obtiene el rango de interacción según la dificultad
     * @return Rango en píxeles
     */
    float getRangoInteraccion() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 120.0f;
            case Dificultad::NORMAL:  return 80.0f;
            case Dificultad::DIFICIL: return 50.0f;
            default: return 80.0f;
        }
    }
    
    /**
     * @brief Obtiene puntos por letra correcta según dificultad
     */
    int getPuntosPorLetra() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 50;
            case Dificultad::NORMAL:  return 100;
            case Dificultad::DIFICIL: return 200;
            default: return 100;
        }
    }
    
    /**
     * @brief Obtiene penalización por error según dificultad
     */
    int getPenalizacionError() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 5;
            case Dificultad::NORMAL:  return 10;
            case Dificultad::DIFICIL: return 25;
            default: return 10;
        }
    }
    
    // =========================================================================
    // CONTROL DE DIFICULTAD
    // =========================================================================
    
    /**
     * @brief Obtiene el nivel de dificultad actual
     */
    Dificultad getDificultadActual() const { return nivelActual; }
    
    /**
     * @brief Obtiene el nombre del nivel de dificultad
     */
    QString getNombreDificultad() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return "FÁCIL";
            case Dificultad::NORMAL:  return "NORMAL";
            case Dificultad::DIFICIL: return "DIFÍCIL";
            default: return "NORMAL";
        }
    }
    
    /**
     * @brief Establece manualmente el nivel de dificultad
     */
    void setDificultad(Dificultad nivel) {
        nivelActual = nivel;
        qDebug() << "[DifficultyManager] Dificultad establecida:" << getNombreDificultad();
    }
    
    /**
     * @brief Registra una ronda completada y ajusta dificultad si corresponde
     */
    void registrarRondaCompletada() {
        rondasCompletadas++;
        qDebug() << "[DifficultyManager] Rondas completadas:" << rondasCompletadas;
        
        // Progresión automática de dificultad
        if (rondasCompletadas >= 3 && nivelActual == Dificultad::FACIL) {
            nivelActual = Dificultad::NORMAL;
            qDebug() << "[DifficultyManager] ¡Dificultad aumentada a NORMAL!";
        } else if (rondasCompletadas >= 6 && nivelActual == Dificultad::NORMAL) {
            nivelActual = Dificultad::DIFICIL;
            qDebug() << "[DifficultyManager] ¡Dificultad aumentada a DIFÍCIL!";
        }
    }
    
    /**
     * @brief Reinicia el progreso de dificultad
     */
    void reiniciar() {
        nivelActual = Dificultad::FACIL;
        rondasCompletadas = 0;
        qDebug() << "[DifficultyManager] Progreso reiniciado";
    }
    
    /**
     * @brief Obtiene el número de rondas completadas
     */
    int getRondasCompletadas() const { return rondasCompletadas; }

private:
    DifficultyManager() 
        : nivelActual(Dificultad::FACIL), 
          rondasCompletadas(0) 
    {
        qDebug() << "[DifficultyManager] Inicializado en dificultad FÁCIL";
    }
    
    ~DifficultyManager() {}
    DifficultyManager(const DifficultyManager&) = delete;
    DifficultyManager& operator=(const DifficultyManager&) = delete;

    Dificultad nivelActual;
    int rondasCompletadas;
};

#endif // DIFFICULTYMANAGER_H
