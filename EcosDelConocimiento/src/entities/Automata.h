#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <QObject>
#include <QPixmap>
#include <QVector>
#include <QPainter>
#include "Entidad.h"  // Herencia propia

class Letra;

/**
 * @brief Clase que representa al NPC autómata del juego.
 * 
 * Hereda de:
 * - QObject: Para señales/slots de Qt
 * - Entidad: Herencia propia (requisito del proyecto)
 */
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
    void setColisiones(const QVector<QRect> &mesas) { colisiones = mesas; }
    void setAreaMovimiento(const QRect &area) { areaMovimiento = area; }

    // Actualización con letras disponibles
    void actualizar(float deltaTime, QVector<Letra*> &letras);

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
    
    // NUEVO: Para pathfinding mejorado
    int direccionRodeo;       // 1 = derecha/abajo, -1 = izquierda/arriba, 0 = ninguna
    float tiempoAtascado;     // Tiempo que lleva sin moverse
    float ultimaPosX;         // Para detectar si está atascado
    float ultimaPosY;

    static const int ANCHO_AUTOMATA = 64;
    static const int ALTO_AUTOMATA = 64;
    static constexpr float RANGO_VISION = 1000.0f;
    static constexpr float RANGO_RECOGIDA = 80.0f;

    // Métodos privados
    bool hayColision(float x, float y, int ancho, int alto);
    void restriccionPantalla();
    QPixmap getSpriteActual() const;
    void cambiarFrame();
    void buscarLetra(QVector<Letra*> &letras);
    void moverHaciaLetra(float deltaTime);
    void subirLetraArriba(float deltaTime);
    bool verificarMovimiento(float nuevoX, float nuevoY, int ancho, int alto);
    bool buscarRutaAlternativa(float &nuevoX, float &nuevoY, float targetX, float targetY, int ancho, int alto);
    void moverConColisiones(float targetX, float targetY, float deltaTime);
    Letra* encontrarLetraCercana(QVector<Letra*> &letras);
    
    // NUEVO: Métodos de pathfinding mejorado
    void actualizarDireccionSprite(float dx, float dy);
    bool dentroLimites(float x, float y);
};

#endif // AUTOMATA_H
