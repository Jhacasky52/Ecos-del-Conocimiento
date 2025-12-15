#ifndef LETRA_H
#define LETRA_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QRect>
#include "Entidad.h"  // Herencia propia

//
// Clase que representa una letra del puzzle.
// 
// Hereda de:
// - QObject: Para señales/slots de Qt
// - Entidad: Herencia propia
// 
// Implementa física de:
// - Caída libre (gravedad)
// - Rebote con fricción
// - Movimiento parabólico (lanzamiento)

class Letra : public QObject, public Entidad
{
    Q_OBJECT

public:
    explicit Letra(QObject *parent = nullptr);
    ~Letra() override;

    // =========================================================================
    // IMPLEMENTACIÓN DE MÉTODOS VIRTUALES DE ENTIDAD
    // =========================================================================
    void actualizar(float deltaTime) override;
    void dibujar(QPainter &painter) override;

    // =========================================================================
    // GETTERS ESPECÍFICOS
    // =========================================================================
    char getLetra() const { return letra; }
    bool estaRecogida() const { return recogida; }
    float getVelocidadY() const { return velocidadY; }

    // =========================================================================
    // SETTERS ESPECÍFICOS
    // =========================================================================
    void setLetra(char l) { letra = l; }
    void setTamanio(int ancho, int alto) { anchoLetra = ancho; altoLetra = alto; }
    void setRecogida(bool r) { recogida = r; }

    // =========================================================================
    // FÍSICA
    // =========================================================================
    
    //
    // Inicia la caída de la letra (usado al crear)
    
    void iniciarCaida(float pisoDestino);
    
    //
    // Suelta la letra con movimiento parabólico
    
    void setSoltada(float posXSoltada, float posYSoltada);
    
    //
    // Suelta la letra SIN movimiento parabólico (para autómata)
    // * La letra simplemente cae hacia abajo desde donde se suelta
    
    void setSoltadaSinMovimiento(float posXSoltada, float posYSoltada);
    
    //
    // Lanza la letra con movimiento parabólico
    // Param: anguloGrados Ángulo de lanzamiento (0-90)
    // Param: velocidadInicial Velocidad inicial del lanzamiento
    
    void lanzar(float anguloGrados, float velocidadInicial);
    
    //
    // Detiene completamente la letra
    
    void detener();
    
    //
    // Reaparece desde arriba en posición aleatoria
    
    void resetearPosicion();

    // =========================================================================
    // COLISIÓN
    // =========================================================================
    QRect getBoundingBox() const;
    bool colisiona(float x, float y) const;

    //
    // Dibuja la letra usando el sprite sheet
    
    void dibujar(QPainter &painter, const QPixmap &spriteLetras);

private:
    char letra;
    int anchoLetra;
    int altoLetra;
    bool recogida;
    bool enReposo;       // Si la letra está quieta (no aplica física)

    // Física de caída
    float pisoDinamico;  // Posición Y donde se detiene la letra

    // Constantes físicas
    static const float GRAVEDAD;
    static const float FRICCION_REBOTE;
    static const float VELOCIDAD_MINIMA_REBOTE;
    static const float FRICCION_AIRE;  // Para movimiento parabólico

    void getLetraCoords(char l, int &col, int &row) const;
};

#endif // LETRA_H
