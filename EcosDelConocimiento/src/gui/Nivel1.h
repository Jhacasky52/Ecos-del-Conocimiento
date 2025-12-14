#ifndef NIVEL1_H
#define NIVEL1_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QVector>
#include "Jugador.h"
#include "Letra.h"
#include "Automata.h"

/**
 * @brief Clase que gestiona el Nivel 1 del juego.
 * 
 * Responsabilidades:
 * - Gestión de entidades (jugador, autómata, letras)
 * - Lógica del puzzle de palabras
 * - Sistema de colisiones
 * - Renderizado del nivel
 */
class Nivel1 : public QObject
{
    Q_OBJECT

public:
    explicit Nivel1(QObject *parent = nullptr);
    ~Nivel1();
    
    static const int NUM_LETRAS_TOTALES = 25;

    // =========================================================================
    // MÉTODOS PRINCIPALES
    // =========================================================================
    void actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime);
    void dibujar(QPainter &painter);
    
    // =========================================================================
    // INTERACCIONES
    // =========================================================================
    void intentarInteractuarConAutomata();
    void intentarRecogerLetra();
    void intentarRecogerLetraConClick(float mouseX, float mouseY);
    void soltarUltimaLetra();
    void verificarLetraEntregada();
    
    // =========================================================================
    // GETTERS
    // =========================================================================
    QString getPalabraObjetivo() const { return palabraObjetivo; }
    int getLetraActualIndex() const { return letraActualIndex; }
    float getTiempoRestante() const { return tiempoRestante; }
    bool esJuegoActivo() const { return juegoActivo; }
    int getPuntuacion() const { return puntuacion; }
    Jugador* getJugador() const { return jugador; }
    QPixmap getSpriteImprentaActual() const;

private:
    // Entidades
    Jugador* jugador;
    Automata* automata;
    QVector<Letra*> letras;

    // Sprites
    QPixmap fondoOriginal;
    QPixmap fondoEscalado;
    QPixmap spriteImprenta;
    QPixmap spriteMesaArriba;
    QPixmap spriteMesaAbajo;
    QPixmap spriteLetras;
    QPixmap spriteImprentaBase;
    QPixmap spriteImprentaAccionada;

    // Área de movimiento
    QRect areaMovimiento;

    // Posición de la imprenta
    float imprentaPosX;
    float imprentaPosY;
    float imprentaAncho;
    float imprentaAlto;
    float tiempoAccionImprenta;
    bool imprentaEnAccion;
    static const float DURACION_ACCION_IMPRENTA;

    // Colisiones
    QVector<QRect> colisiones;

    // Sistema de juego
    QString palabraObjetivo;
    int letraActualIndex;
    float tiempoRestante;
    bool juegoActivo;
    int puntuacion;
    static const float TIEMPO_LIMITE;

    // Estructura para ordenar objetos por profundidad
    struct ObjetoConProfundidad {
        int tipo;
        float y;
        int id;
    };

    // Métodos privados
    void iniciarJuego();
    void actualizarJuego(float deltaTime);
    void finalizarJuego(bool victoria);
    void dibujarGameUI(QPainter &painter);
    void dibujarFondo(QPainter &painter);
    void dibujarObjetos(QPainter &painter);
    void dibujarUI(QPainter &painter);
    void dibujarAreaMovimiento(QPainter &painter);
    void dibujarColisiones(QPainter &painter);
    void definirAreaMovimiento();
    void definirColisiones();
    void cargarSpritesObjetos();
    void crearLetras();
    bool hayColision(float x, float y, int ancho, int alto);
};

#endif // NIVEL1_H
