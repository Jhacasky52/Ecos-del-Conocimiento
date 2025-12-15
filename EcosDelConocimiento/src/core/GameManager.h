#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>

//
//Gestor principal del juego.
// 
// Controla el flujo general del juego:
// - Estados del juego (menú, jugando, pausa, game over)
// - Transiciones entre niveles
// - Guardado/carga de progreso

class GameManager : public QObject
{
    Q_OBJECT

public:
    enum EstadoJuego {
        MENU,
        JUGANDO,
        PAUSA,
        GAME_OVER,
        VICTORIA
    };

    static GameManager& getInstance() {
        static GameManager instance;
        return instance;
    }

    EstadoJuego getEstado() const { return estadoActual; }
    void setEstado(EstadoJuego estado) { estadoActual = estado; }
    
    int getNivelActual() const { return nivelActual; }
    void setNivelActual(int nivel) { nivelActual = nivel; }
    
    void iniciarNuevoJuego();
    void pausar();
    void reanudar();
    void terminarJuego();

signals:
    void estadoCambiado(EstadoJuego nuevoEstado);
    void nivelCambiado(int nuevoNivel);

private:
    GameManager(QObject *parent = nullptr);
    ~GameManager() = default;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    EstadoJuego estadoActual;
    int nivelActual;
};

#endif // GAMEMANAGER_H
