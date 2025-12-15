#ifndef AUTOMATA_H

#define AUTOMATA_H

#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QPoint>
#include <QVector>
#include <QMap>

#include "Entidad.h"

class Letra;

// Clase que representa al NPC autómata del juego.
// Implementa el ciclo PENSAR -> DECIDIR -> ACTUAR -> MEMORIZAR
//
// Hereda de:
// - QObject: Para señales/slots de Qt
// - Entidad: Herencia propia (requisito del proyecto)
//
// Usa Pathfinding con A* simplificado usando grid de navegación

class Automata : public QObject, public Entidad
{
    Q_OBJECT

public:
    enum Direccion {
        FRENTE,
        ESPALDA,
        IZQUIERDA,
        DERECHA
    };

    explicit Automata(QObject *parent = nullptr);
    ~Automata() override;

    // =========================================================================
    // IMPLEMENTACIÓN DE MÉTODOS VIRTUALES DE ENTIDAD
    // =========================================================================

    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // =========================================================================
    // MÉTODOS ESPECÍFICOS DEL AUTÓMATA
    // =========================================================================

    void interactuarConJugador(char letraFaltante);

    bool estaActivoAutomata() const { return automataActivo; }
    void setActivoAutomata(bool estado) { automataActivo = estado; }
    Direccion getDireccion() const { return direccion; }
    void setVelocidadMovimiento(float vel) { velocidadMovimiento = vel; }
    void setLimites(float ancho, float alto);
    void cargarSprites();
    void setColisiones(const QVector<QRect> &mesas) { colisiones = mesas; generarGridNavegacion(); }
    void setAreaMovimiento(const QRect &area) { areaMovimiento = area; }

    // Actualización con letras disponibles
    void actualizar(float deltaTime, QVector<Letra*> &letras);

    //  Variables para letra pegada
    Letra* letraEnMano;
    float offsetLetraX;
    float offsetLetraY;

    // Funciones para colisión
    bool verificarColisionMesa(float testX, float testY, float margen);
    std::vector<QRect> obtenerMesasDelNivel() const;

    // Letras recogidas
    QVector<Letra*> getLetrasRecogidas() const { return letrasRecogidas; }
    void agregarLetra(Letra *letra) { letrasRecogidas.append(letra); }
    void soltarLetra(Letra *letra) { letrasRecogidas.removeAll(letra); }

private:

    // Estado del autómata
    bool automataActivo;

    // Movimiento
    float velocidadMovimiento;
    Direccion direccion;
    float anchoMundo;
    float altoMundo;
    QVector<QRect> colisiones;
    QRect areaMovimiento;

    // Sprites
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

    // Control de comportamiento automático
    float tiempoHastaProximaAccion;
    Letra* letraObjetivo;
    char letraABuscar;
    bool tieneMision;
    bool buscandoLetra;
    float posicionXArriba;

    // Para subir/bajar
    bool estaBajando;
    float posicionDesdeQueBaja;

    // =========================================================================
    // SISTEMA DE PATHFINDING A* MEJORADO
    // =========================================================================

    static const int GRID_SIZE = 32; // Tamaño de celda del grid
    static const int GRID_WIDTH = 32; // 1024 / 32 = 32 celdas de ancho
    static const int GRID_HEIGHT = 24; // 768 / 32 = 24 celdas de alto

    bool gridNavegacion[GRID_WIDTH][GRID_HEIGHT]; // true = navegable, false = obstáculo
    QVector<QPoint> rutaActual; // Ruta calculada con A*
    int indicePuntoRuta; // Índice del punto actual en la ruta
    float tiempoRecalculoRuta; // Timer para recalcular ruta
    QPoint ultimoObjetivo; // Última posición objetivo

    // Para detección de atasco
    float tiempoAtascado;
    float ultimaPosX;
    float ultimaPosY;
    int intentosFallidos;

    static const int ANCHO_AUTOMATA = 40;
    static const int ALTO_AUTOMATA = 40;


    static constexpr float RANGO_VISION = 1000.0f;
    static constexpr float RANGO_RECOGIDA = 120.0f;

    // =========================================================================
    // MÉTODOS PRIVADOS - NAVEGACIÓN Y PATHFINDING
    // =========================================================================

    void generarGridNavegacion();
    bool celdaNavegable(int gx, int gy) const;
    QPoint posicionAGrid(float x, float y) const;
    QPointF gridAPosicion(int gx, int gy) const;

    // A* Pathfinding
    QVector<QPoint> calcularRutaAStar(QPoint inicio, QPoint fin);
    int heuristica(QPoint a, QPoint b) const;
    QVector<QPoint> obtenerVecinos(QPoint p) const;
    QVector<QPoint> reconstruirRuta(QMap<QPoint, QPoint> &cameFrom, QPoint actual);

    // Movimiento suave hacia waypoint
    bool moverHaciaWaypoint(float deltaTime);

    // =========================================================================
    // MÉTODOS PRIVADOS - OTROS
    // =========================================================================

    bool hayColision(float x, float y, int ancho, int alto);
    void restriccionPantalla();
    QPixmap getSpriteActual() const;
    void cambiarFrame();
    void subirLetraArriba(float deltaTime);
    Letra* encontrarLetraCercana(QVector<Letra*> &letras);
    void actualizarDireccionSprite(float dx, float dy);
    bool dentroLimites(float x, float y);
};

// Operador necesario para QMap con QPoint
inline bool operator<(const QPoint &a, const QPoint &b) {
    if (a.x() != b.x()) return a.x() < b.x();
    return a.y() < b.y();
}

#endif // AUTOMATA_H
