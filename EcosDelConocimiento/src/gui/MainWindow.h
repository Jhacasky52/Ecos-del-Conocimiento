#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSet>
#include <QRect>
#include <QPixmap>

class QTimer;
class Nivel1;
class Nivel2;
class Nivel3;

//
// Ventana principal del juego.
// 
// Maneja:
// - Loop principal del juego (60 FPS)
// - Entrada de teclado y mouse
// - Renderizado
// - Gestión de los 3 niveles
// - Sistema de pausa (ESC)
// - Pantallas de victoria y derrota
// - Flujo coherente entre niveles
// - Puntajes acumulados
// - Control de sonido on/off

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(int nivelInicial = 1, QWidget *parent = nullptr);
    ~MainWindow();

    // Estados del juego
    enum EstadoJuego {
        JUGANDO,
        PAUSADO,
        VICTORIA,
        DERROTA,
        RESUMEN_FINAL
    };

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void volverAlMenu();

private slots:
    void actualizarJuego();

private:
    // Niveles (solo uno activo a la vez)
    Nivel1 *nivel1;
    Nivel2 *nivel2;
    Nivel3 *nivel3;
    
    int nivelActual;
    int nivelMaximoDesbloqueado;  // Para el modo historia
    
    QTimer *timer;
    QSet<int> teclasPresionadas;
    
    // Estado del juego
    EstadoJuego estadoJuego;
    
    // Puntajes por nivel
    int puntajeNivel1;
    int puntajeNivel2;
    int puntajeNivel3;
    bool nivel1Completado;
    bool nivel2Completado;
    bool nivel3Completado;
    
    // Fondos para pantallas de estado
    QPixmap fondoVictoria;
    QPixmap fondoDerrota;
    QPixmap fondoPausa;
    
    // Sprites de sonido on/off
    QPixmap spriteSoundOn;
    QPixmap spriteSoundOff;
    QRect btnSound;
    bool hoverSound;
    
    // Botones de las pantallas de estado
    QRect btnAvanzar;
    QRect btnSalirVictoria;
    QRect btnContinuar;
    QRect btnSalirPausa;
    QRect btnMenuDerrota;
    QRect btnSalirDerrota;
    QRect btnMenuFinal;
    QRect btnSalirFinal;
    
    // Hover de botones
    bool hoverAvanzar;
    bool hoverSalirVictoria;
    bool hoverContinuar;
    bool hoverSalirPausa;
    bool hoverMenuDerrota;
    bool hoverSalirDerrota;
    bool hoverMenuFinal;
    bool hoverSalirFinal;
    
    // Métodos de inicialización
    void inicializarNivel(int nivel);
    void limpiarNiveles();
    void cargarFondosEstado();
    void cargarSpritesSonido();
    void definirBotonesEstado();
    
    // Métodos de dibujo de pantallas de estado
    void dibujarPantallaPausa(QPainter &painter);
    void dibujarPantallaVictoria(QPainter &painter);
    void dibujarPantallaDerrota(QPainter &painter);
    void dibujarResumenFinal(QPainter &painter);
    void dibujarBotonEstado(QPainter &painter, const QRect &rect, const QString &texto, 
                            bool hover, const QColor &colorBase);
    void dibujarBotonSonido(QPainter &painter);
    
    // Verificaciones
    void verificarEstadoNivel();
    void verificarHoverPausa(int x, int y);
    void verificarHoverVictoria(int x, int y);
    void verificarHoverDerrota(int x, int y);
    void verificarHoverFinal(int x, int y);
    void verificarHoverSound(int x, int y);
    
    // Acciones
    void pausarJuego();
    void continuarJuego();
    void avanzarSiguienteNivel();
    void mostrarResumenFinal();
    void volverAlMenuPrincipal();
    void salirDelJuego();
    void toggleSonido();
};

#endif // MAINWINDOW_H
