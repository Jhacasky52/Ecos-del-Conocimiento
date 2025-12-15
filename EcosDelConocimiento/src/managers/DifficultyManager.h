#ifndef DIFFICULTYMANAGER_H
#define DIFFICULTYMANAGER_H

#include <QDebug>
#include <QString>

// Gestor de dificultad del juego (Singleton)
// Implementa el sistema de dificultad progresiva
// Controla: tiempo límite, velocidad del jugador, número de letras,
// velocidad del autómata y progresión automática de dificultad

enum class Dificultad {
    FACIL,
    NORMAL,
    DIFICIL
};

class DifficultyManager {
public:
    // Obtiene la instancia única del DifficultyManager
    static DifficultyManager& getInstance() {
        static DifficultyManager instance;
        return instance;
    }
    
    // Obtiene el tiempo límite según la dificultad (en segundos)
    float getTiempoLimite() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 90.0f;
            case Dificultad::NORMAL:  return 60.0f;
            case Dificultad::DIFICIL: return 40.0f;
            default: return 60.0f;
        }
    }
    
    // Obtiene la velocidad del jugador según la dificultad (píxeles/segundo)
    float getVelocidadJugador() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 350.0f;
            case Dificultad::NORMAL:  return 300.0f;
            case Dificultad::DIFICIL: return 250.0f;
            default: return 300.0f;
        }
    }
    
    // Obtiene el número de letras según la dificultad
    int getNumeroLetras() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 25;
            case Dificultad::NORMAL:  return 25;
            case Dificultad::DIFICIL: return 25;
            default: return 25;
        }
    }
    
    // Obtiene la velocidad del autómata según la dificultad (píxeles/segundo)
    float getVelocidadAutomata() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 150.0f;
            case Dificultad::NORMAL:  return 200.0f;
            case Dificultad::DIFICIL: return 250.0f;
            default: return 200.0f;
        }
    }
    
    // Obtiene el rango de interacción según la dificultad (píxeles)
    float getRangoInteraccion() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 160.0f;
            case Dificultad::NORMAL:  return 140.0f;
            case Dificultad::DIFICIL: return 120.0f;
            default: return 160.0f;
        }
    }
    
    // Obtiene puntos por letra correcta según dificultad
    int getPuntosPorLetra() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 50;
            case Dificultad::NORMAL:  return 100;
            case Dificultad::DIFICIL: return 200;
            default: return 100;
        }
    }
    
    // Obtiene penalización por error según dificultad
    int getPenalizacionError() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 5;
            case Dificultad::NORMAL:  return 10;
            case Dificultad::DIFICIL: return 25;
            default: return 10;
        }
    }
    
    // =========================================================================
    // PARÁMETROS ESPECÍFICOS NIVEL 2 - BIBLIOTECA EN LLAMAS
    // =========================================================================
    
    // Obtiene el tiempo límite del Nivel 2 según dificultad
    float getTiempoLimiteNivel2() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 240.0f;  // 4 minutos
            case Dificultad::NORMAL:  return 180.0f;  // 3 minutos
            case Dificultad::DIFICIL: return 120.0f;  // 2 minutos
            default: return 180.0f;
        }
    }
    
    // Obtiene el intervalo entre spawns de fuego
    float getIntervaloSpawnFuego() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 2.5f;    // Fuego más lento
            case Dificultad::NORMAL:  return 1.5f;
            case Dificultad::DIFICIL: return 0.8f;    // Fuego muy frecuente
            default: return 1.5f;
        }
    }
    
    // Obtiene el intervalo entre spawns de pergaminos
    float getIntervaloSpawnPergamino() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 3.0f;    // Pergaminos más frecuentes
            case Dificultad::NORMAL:  return 4.0f;
            case Dificultad::DIFICIL: return 5.0f;    // Menos pergaminos
            default: return 4.0f;
        }
    }
    
    // Obtiene el tiempo de vida del fuego en el piso
    float getTiempoFuegoPiso() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 2.0f;    // Fuego dura menos
            case Dificultad::NORMAL:  return 3.0f;
            case Dificultad::DIFICIL: return 5.0f;    // Fuego dura más
            default: return 3.0f;
        }
    }
    
    // Obtiene la velocidad del cuervo
    float getVelocidadCuervo() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 120.0f;  // Cuervo lento
            case Dificultad::NORMAL:  return 180.0f;
            case Dificultad::DIFICIL: return 250.0f;  // Cuervo muy rápido
            default: return 180.0f;
        }
    }
    
    // Obtiene los pergaminos necesarios para ganar
    int getPergaminosParaGanar() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 4;
            case Dificultad::NORMAL:  return 6;
            case Dificultad::DIFICIL: return 8;
            default: return 6;
        }
    }
    
    // Obtiene puntos por pergamino entregado
    int getPuntosPorPergamino() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return 100;
            case Dificultad::NORMAL:  return 150;
            case Dificultad::DIFICIL: return 250;
            default: return 150;
        }
    }
    
    // Obtiene el nivel de dificultad actual
    Dificultad getDificultadActual() const { return nivelActual; }
    
    // Obtiene el nombre del nivel de dificultad
    QString getNombreDificultad() const {
        switch(nivelActual) {
            case Dificultad::FACIL:   return "FÁCIL";
            case Dificultad::NORMAL:  return "NORMAL";
            case Dificultad::DIFICIL: return "DIFÍCIL";
            default: return "NORMAL";
        }
    }
    
    // Establece manualmente el nivel de dificultad
    void setDificultad(Dificultad nivel) {
        nivelActual = nivel;
        qDebug() << "[DifficultyManager] Dificultad establecida:" << getNombreDificultad();
    }
    
    // Establece la dificultad usando un entero (0=Fácil, 1=Normal, 2=Difícil)
    void setDificultad(int nivel) {
        switch(nivel) {
            case 0: nivelActual = Dificultad::FACIL; break;
            case 1: nivelActual = Dificultad::NORMAL; break;
            case 2: nivelActual = Dificultad::DIFICIL; break;
            default: nivelActual = Dificultad::NORMAL; break;
        }
        qDebug() << "[DifficultyManager] Dificultad establecida:" << getNombreDificultad();
    }
    
    // Obtiene la dificultad como entero (0=Fácil, 1=Normal, 2=Difícil)
    int getDificultad() const {
        switch(nivelActual) {
            case Dificultad::FACIL: return 0;
            case Dificultad::NORMAL: return 1;
            case Dificultad::DIFICIL: return 2;
            default: return 1;
        }
    }
    
    // Registra una ronda completada y ajusta dificultad si corresponde
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
    
    // Reinicia el progreso de dificultad
    void reiniciar() {
        nivelActual = Dificultad::FACIL;
        rondasCompletadas = 0;
        qDebug() << "[DifficultyManager] Progreso reiniciado";
    }
    
    // Obtiene el número de rondas completadas
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
