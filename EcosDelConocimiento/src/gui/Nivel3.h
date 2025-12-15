#ifndef NIVEL3_H
#define NIVEL3_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QVector>
#include "Jugador.h"


//Nivel 3 - La Revolución del Saber (Ciberespacio)
// 
// Mecánicas:
// - Recolectar fragmentos de conocimiento y entregarlos al terminal
// - Evadir virus y obstáculos (firewalls)
// - Regenerar energía al pasar por nodos de verificación
// 
// Físicas:
// - Movimiento oscilatorio de virus
// - Campos de atracción/repulsión
// - Rebote con coeficiente de restitución en colisiones
// - Simulación de fuerza de campo eléctrico
// 
// Guardian AI: Protege al jugador, ayuda cuando detecta peligro

class Nivel3 : public QObject
{
    Q_OBJECT

public:
    explicit Nivel3(QObject *parent = nullptr);
    ~Nivel3();

    // Métodos principales
    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void dibujar(QPainter &painter);
    
    // Interacciones
    void intentarRecogerDato();
    void intentarRecogerDatoConClick(float mouseX, float mouseY);
    void soltarUltimoDato();
    void verificarDatoEntregado();
    void intentarInteractuarConAutomata();
    void activarEscudo();
    
    // Getters
    QString getCodigoObjetivo() const { return ""; }  // No usado
    int getBitActualIndex() const { return 0; }        // No usado
    float getTiempoRestante() const { return tiempoRestante; }
    bool esJuegoActivo() const { return juegoActivo; }
    int getPuntuacion() const { return puntuacion; }
    Jugador* getJugador() const { return jugador; }
    float getEnergia() const { return energia; }
    bool tieneEscudoActivo() const { return escudoActivo; }
    int getFragmentosEntregados() const { return fragmentosEntregados; }

signals:
    void nivelCompletado(int puntuacionFinal);
    void nivelFallido();

private:
    // =========================================================================
    // JUGADOR (Packet)
    // =========================================================================
    Jugador* jugador;
    float velocidadJugadorX, velocidadJugadorY;
    
    // Sprites del jugador
    QPixmap spritePacket;
    QPixmap spritePacketEscudo;
    
    // =========================================================================
    // SPRITES
    // =========================================================================
    QPixmap fondoNivel;
    QPixmap spriteVirus;
    QPixmap spriteFragmento;
    QPixmap spriteNodo;
    QPixmap spriteFirewall;
    QPixmap spriteTerminal;
    QPixmap spriteGuardian;
    
    // =========================================================================
    // FRAGMENTOS DE CONOCIMIENTO (ahora con movimiento aleatorio tipo virus)
    // =========================================================================
    struct Fragmento {
        float x, y;
        bool recogido;
        bool entregado;
        float oscilacionFase;
        // Nuevos campos para movimiento tipo virus
        float velocidadBaseX, velocidadBaseY;
        float amplitud;
        float frecuencia;
    };
    QVector<Fragmento> fragmentos;
    int fragmentoRecogidoIndex;  // -1 si no tiene ninguno
    int fragmentosEntregados;
    static constexpr int FRAGMENTOS_PARA_GANAR = 5;
    static constexpr int NUM_FRAGMENTOS = 8;
    
    // =========================================================================
    // VIRUS (Enemigos con movimiento oscilatorio)
    // =========================================================================
    struct Virus {
        float x, y;
        float velocidadBaseX, velocidadBaseY;
        float amplitud;
        float frecuencia;
        float fase;
        float radio;
    };
    QVector<Virus> virus;
    static constexpr int NUM_VIRUS = 6;
    
    // =========================================================================
    // NODOS DE VERIFICACIÓN (Regeneran energía)
    // =========================================================================
    struct Nodo {
        float x, y;
        float radio;
        bool activo;
        float tiempoRecarga;
        float pulso;
    };
    QVector<Nodo> nodos;
    static constexpr int NUM_NODOS = 4;
    static constexpr float NODO_RECARGA_TIEMPO = 8.0f;
    static constexpr float NODO_ENERGIA = 30.0f;
    
    // =========================================================================
    // FIREWALLS (Obstáculos con rebote)
    // =========================================================================
    struct Firewall {
        float x, y;
        float ancho, alto;
        bool movil;
        float velocidadY;
        float limiteInferior, limiteSuperior;
    };
    QVector<Firewall> firewalls;
    static constexpr int NUM_FIREWALLS = 3;
    static constexpr float COEFICIENTE_RESTITUCION = 0.7f;
    
    // =========================================================================
    // TERMINAL (Punto de entrega)
    // =========================================================================
    float terminalX, terminalY;
    float terminalAncho, terminalAlto;
    float pulsoTerminal;
    
    // =========================================================================
    // GUARDIAN AI - AHORA ES ENEMIGO
    // =========================================================================
    float guardianX, guardianY;
    float guardianVelX, guardianVelY;
    bool guardianActivo;
    float guardianPulso;
    static constexpr float GUARDIAN_VELOCIDAD = 180.0f;      // Velocidad de persecución
    static constexpr float GUARDIAN_DANIO = 25.0f;           // Daño al tocar jugador
    static constexpr float GUARDIAN_RADIO = 40.0f;           // Radio de colisión
    
    // =========================================================================
    // CAMPOS DE FUERZA (Atracción/Repulsión)
    // =========================================================================
    struct CampoFuerza {
        float x, y;
        float radio;
        float intensidad;  // Positivo = repulsión, Negativo = atracción
    };
    QVector<CampoFuerza> campos;
    
    // =========================================================================
    // SISTEMA DE ENERGÍA Y ESCUDO
    // =========================================================================
    float energia;
    float energiaMaxima;
    bool escudoActivo;
    float tiempoEscudo;
    static constexpr float DURACION_ESCUDO = 3.0f;
    static constexpr float COSTO_ESCUDO = 25.0f;
    static constexpr float DANIO_VIRUS = 20.0f;
    
    // =========================================================================
    // FÍSICA
    // =========================================================================
    static constexpr float FRICCION = 0.92f;
    static constexpr float VELOCIDAD_MAXIMA = 400.0f;
    static constexpr float ACELERACION = 800.0f;
    
    // =========================================================================
    // EFECTOS VISUALES
    // =========================================================================
    struct Particula {
        float x, y;
        float velocidadY;
        char caracter;
        float alpha;
    };
    QVector<Particula> particulas;
    float offsetGrid;
    
    // =========================================================================
    // SISTEMA DE JUEGO
    // =========================================================================
    float tiempoRestante;
    bool juegoActivo;
    int puntuacion;
    float tiempoInvulnerable;
    static constexpr float TIEMPO_LIMITE = 120.0f;
    
    // =========================================================================
    // MÉTODOS PRIVADOS
    // =========================================================================
    void iniciarJuego();
    void cargarRecursos();
    void crearFragmentos();
    void crearVirus();
    void crearNodos();
    void crearFirewalls();
    void crearCamposFuerza();
    void generarParticulas();
    
    void actualizarJuego(float deltaTime);
    void actualizarFisicaJugador(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void actualizarVirus(float deltaTime);
    void actualizarFragmentos(float deltaTime);  // NUEVO - Movimiento de fragmentos
    void actualizarNodos(float deltaTime);
    void actualizarFirewalls(float deltaTime);
    void actualizarGuardian(float deltaTime);
    void actualizarParticulas(float deltaTime);
    void aplicarCamposFuerza(float deltaTime);
    
    void verificarColisionesVirus();
    void verificarColisionesFirewall();
    void verificarColisionGuardian();  // NUEVO - Colisión con guardián enemigo
    void verificarNodos();
    
    void dibujarFondo(QPainter &painter);
    void dibujarGrid(QPainter &painter);
    void dibujarParticulas(QPainter &painter);
    void dibujarCampos(QPainter &painter);
    void dibujarFirewalls(QPainter &painter);
    void dibujarNodos(QPainter &painter);
    void dibujarFragmentos(QPainter &painter);
    void dibujarVirus(QPainter &painter);
    void dibujarTerminal(QPainter &painter);
    void dibujarJugador(QPainter &painter);
    void dibujarGuardian(QPainter &painter);
    void dibujarUI(QPainter &painter);
    
    void finalizarJuego(bool victoria);
    float distancia(float x1, float y1, float x2, float y2);
};

#endif // NIVEL3_H
