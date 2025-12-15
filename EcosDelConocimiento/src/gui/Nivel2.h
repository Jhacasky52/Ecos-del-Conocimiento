#ifndef NIVEL2_H
#define NIVEL2_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QVector>
#include "Jugador.h"


//Nivel 2 - La Biblioteca en Llamas
// 
// Mecánicas:
// - Pergaminos caen del cielo (recogerlos y llevarlos al cofre)
// - Fuego cae del cielo y se esparce (3 sprites adyacentes en el piso)
// - Pergaminos se prenden al tocar fuego (10 segundos para quemarse)
// - Jugador puede tomar cubeta de agua de la fuente y lanzarla (parábola)
// - Jugador puede saltar sobre máximo 2 sprites de fuego
// - Fuente a la izquierda, Cofre a la derecha
// - Todo se mueve con el parallax cercano
// - Ganar: entregar 6 pergaminos al cofre

class Nivel2 : public QObject
{
    Q_OBJECT

public:
    explicit Nivel2(QObject *parent = nullptr);
    ~Nivel2();

    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void dibujar(QPainter &painter);
    
    // Interacciones
    void intentarSaltar();
    void intentarRecogerPergamino();
    void soltarPergamino();
    void intentarTomarAgua();
    void lanzarCubeta();
    void entregarPergamino();
    
    // Getters
    float getTiempoRestante() const { return tiempoRestante; }
    bool esJuegoActivo() const { return juegoActivo; }
    int getPuntuacion() const { return puntuacion; }
    Jugador* getJugador() const { return jugador; }
    int getPergaminosEntregados() const { return pergaminosEntregados; }
    bool tieneCubeta() const { return jugadorTieneCubeta; }

signals:
    void nivelCompletado(int puntuacionFinal);
    void nivelFallido();

private:
    // =========================================================================
    // ENTIDADES
    // =========================================================================
    Jugador* jugador;

    // =========================================================================
    // SPRITES DEL JUGADOR (2 frames por sprite sheet 612x408)
    // =========================================================================
    QPixmap spriteJugadorDerecha;
    QPixmap spriteJugadorIzquierda;
    QPixmap spriteSaltoDerecha;
    QPixmap spriteSaltoIzquierda;
    
    int frameActual;
    float tiempoFrame;
    static constexpr float TIEMPO_POR_FRAME = 0.15f;
    static constexpr int FRAME_ANCHO = 306;
    static constexpr int FRAME_ALTO = 408;

    // =========================================================================
    // SPRITES DE OBJETOS
    // =========================================================================
    QPixmap spriteFuegoAire;
    QPixmap spriteFuegoPiso;
    QPixmap spritePergamino;
    QPixmap spritePergaminoPocoFuego;
    QPixmap spritePergaminoMuchoFuego;
    QPixmap spriteFuente;
    QPixmap spriteCofre;
    QPixmap spriteCubeta;
    QPixmap spriteCuervoDerecha;
    QPixmap spriteCuervoDerecha2;
    QPixmap spriteCuervoIzquierda;
    QPixmap spriteCuervoIzquierda2;

    // =========================================================================
    // PARALLAX (3 capas)
    // =========================================================================
    QPixmap fondoLejano;
    QPixmap fondoMedio;
    QPixmap fondoCerca;
    
    float offsetMundo;  // Offset del mundo (todo se mueve con esto)
    
    static constexpr float VEL_PARALLAX_LEJANO = 0.3f;
    static constexpr float VEL_PARALLAX_MEDIO = 0.6f;
    static constexpr float VEL_PARALLAX_CERCA = 1.0f;  // El cercano es la referencia

    // =========================================================================
    // FÍSICA
    // =========================================================================
    float velocidadY;
    bool enSuelo;
    bool mirandoDerecha;
    bool seEstaMoviendo;
    static constexpr float GRAVEDAD = 1200.0f;
    static constexpr float FUERZA_SALTO = -600.0f;
    static constexpr float SUELO_Y = 720.0f;  // 100px más abajo
    static constexpr float VELOCIDAD_JUGADOR = 350.0f;
    
    // Límites del mundo
    static constexpr float MUNDO_ANCHO = 2500.0f;
    static constexpr float MUNDO_MIN_X = 0.0f;

    // =========================================================================
    // FUEGO (cae del cielo y se esparce)
    // =========================================================================
    struct FuegoAire {
        float x, y;         // Posición en el mundo
        float velocidadY;
        bool activo;
    };
    QVector<FuegoAire> fuegosAire;
    
    struct FuegoPiso {
        float x;            // Posición X en el mundo
        float tiempoVida;
        bool activo;
    };
    QVector<FuegoPiso> fuegosPiso;
    
    float tiempoSpawnFuego;
    static constexpr float INTERVALO_SPAWN_FUEGO = 1.5f;
    static constexpr float VELOCIDAD_CAIDA_FUEGO = 200.0f;
    static constexpr float TIEMPO_FUEGO_PISO = 3.0f;
    static constexpr float FUEGO_TAMANIO = 73.0f;  // Doble de tamaño

    // =========================================================================
    // PERGAMINOS (caen del cielo)
    // =========================================================================
    enum EstadoPergamino {
        NORMAL,
        POCO_FUEGO,     // Acaba de prenderse
        MUCHO_FUEGO,    // Casi quemado
        QUEMADO         // Destruido
    };
    
    struct Pergamino {
        float x, y;         // Posición en el mundo
        float velocidadY;
        bool enAire;
        bool recogido;
        EstadoPergamino estado;
        float tiempoEnFuego;    // Tiempo que lleva prendiéndose
        bool activo;
    };
    QVector<Pergamino> pergaminos;
    
    float tiempoSpawnPergamino;
    static constexpr float INTERVALO_SPAWN_PERGAMINO = 4.0f;
    static constexpr float VELOCIDAD_CAIDA_PERGAMINO = 140.0f;
    static constexpr float TIEMPO_POCO_FUEGO = 3.0f;    // 5 seg para pasar a mucho fuego
    static constexpr float TIEMPO_MUCHO_FUEGO = 3.0f;   // 5 seg más para quemarse (total 10)
    static constexpr float PERGAMINO_TAMANIO = 73.0f;  // Doble de tamaño
    
    int pergaminoRecogidoIndex;  // -1 si no tiene ninguno

    // =========================================================================
    // CUBETA DE AGUA (física parabólica)
    // =========================================================================
    struct Cubeta {
        float x, y;
        float velocidadX, velocidadY;
        bool activa;
        bool enVuelo;
    };
    Cubeta cubetaLanzada;
    bool jugadorTieneCubeta;
    
    static constexpr float CUBETA_VEL_INICIAL = 400.0f;
    static constexpr float CUBETA_ANGULO = 45.0f;  // Grados
    static constexpr float CUBETA_RADIO_EFECTO = 160.0f;  // Doble de radio

    // =========================================================================
    // FUENTE Y COFRE (posiciones fijas en el mundo)
    // =========================================================================
    float fuenteX;
    float cofreX;
    static constexpr float FUENTE_ANCHO = 150.0f;  // Doble
    static constexpr float COFRE_ANCHO = 150.0f;   // Doble

    // =========================================================================
    // CUERVO AUTÓMATA (roba pergaminos)
    // =========================================================================
    float cuervoX, cuervoY;
    float cuervoVelX, cuervoVelY;
    bool cuervoMirandoDerecha;
    int cuervoFrame;
    float cuervoTiempoFrame;
    
    // Estados del cuervo
    enum EstadoCuervo {
        CUERVO_VOLANDO,      // Buscando pergamino
        CUERVO_BAJANDO,      // Bajando a robar
        CUERVO_ROBANDO,      // Recogiendo pergamino
        CUERVO_ESCAPANDO     // Huyendo con pergamino
    };
    EstadoCuervo estadoCuervo;
    int pergaminoObjetivoCuervo;  // Índice del pergamino que va a robar
    float cuervoTiempoEspera;
    int pergaminosRobados;
    
    static constexpr float CUERVO_VELOCIDAD = 180.0f;
    static constexpr float CUERVO_VELOCIDAD_BAJADA = 250.0f;

    // =========================================================================
    // SISTEMA DE JUEGO
    // =========================================================================
    float tiempoRestante;
    bool juegoActivo;
    int puntuacion;
    int pergaminosEntregados;
    
    static constexpr float TIEMPO_LIMITE = 180.0f;  // 3 minutos
    static constexpr int PERGAMINOS_PARA_GANAR = 6;

    // =========================================================================
    // MÉTODOS PRIVADOS
    // =========================================================================
    void cargarRecursos();
    void iniciarJuego();
    
    void actualizarJugador(bool izquierda, bool derecha, float deltaTime);
    void actualizarAnimacion(float deltaTime);
    void actualizarFuegosAire(float deltaTime);
    void actualizarFuegosPiso(float deltaTime);
    void actualizarPergaminos(float deltaTime);
    void actualizarCubeta(float deltaTime);
    void actualizarCuervo(float deltaTime);
    
    void spawnFuego();
    void spawnPergamino();
    void esparcirFuegoEnPiso(float x);
    void apagarFuegosEnArea(float x, float radio);
    
    bool hayFuegoEnPosicion(float x);
    int contarFuegosEntre(float x1, float x2);
    
    float mundoAPantalla(float xMundo);
    float pantallaAMundo(float xPantalla);
    
    void dibujarFondoParallax(QPainter &painter);
    void dibujarFuente(QPainter &painter);
    void dibujarCofre(QPainter &painter);
    void dibujarFuegosPiso(QPainter &painter);
    void dibujarFuegosAire(QPainter &painter);
    void dibujarPergaminos(QPainter &painter);
    void dibujarCubeta(QPainter &painter);
    void dibujarJugador(QPainter &painter);
    void dibujarCuervo(QPainter &painter);
    void dibujarUI(QPainter &painter);
    
    void finalizarJuego(bool victoria);
};

#endif // NIVEL2_H
