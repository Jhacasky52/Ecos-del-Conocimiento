#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QPixmap>
#include <QRect>

//Menú principal del juego con sistema de navegación completo.

// Estados del menú:
// - MENU_PRINCIPAL: Inicio, Opciones, Salir
// - MENU_OPCIONES: Créditos, Dificultad, Nivel 1/2/3, Volver
// - MENU_CREDITOS: Muestra los créditos
// - MENU_DIFICULTAD: Fácil, Normal, Difícil
// Incluye efectos de hover con sprites precargados y fallbacks
// Incluye botón de sonido on/off en la esquina superior derecha

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

    // Estados del menú
    enum EstadoMenu {
        MENU_PRINCIPAL,
        MENU_OPCIONES,
        MENU_CREDITOS,
        MENU_DIFICULTAD
    };

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    EstadoMenu estadoActual;

    // Fondos
    QPixmap fondoMenu;
    QPixmap fondoOpciones;

    // Sprites de botones del menú principal
    QPixmap btnInicioNormal;
    QPixmap btnInicioHover;
    QPixmap btnOpcionesNormal;
    QPixmap btnOpcionesHover;
    QPixmap btnSalirNormal;
    QPixmap btnSalirHover;

    // Sprites de botones del submenú opciones
    QPixmap btnCreditosNormal;
    QPixmap btnCreditosHover;
    QPixmap btnDificultadNormal;
    QPixmap btnDificultadHover;
    QPixmap btnNivel1Normal;
    QPixmap btnNivel1Hover;
    QPixmap btnNivel2Normal;
    QPixmap btnNivel2Hover;
    QPixmap btnNivel3Normal;
    QPixmap btnNivel3Hover;
    QPixmap btnVolverNormal;
    QPixmap btnVolverHover;

    // Sprites de sonido on/off
    QPixmap spriteSoundOn;
    QPixmap spriteSoundOff;
    QRect btnSound;
    bool hoverSound = false;

    // Rectángulos de botones - Menú Principal
    QRect btnInicio;
    QRect btnOpciones;
    QRect btnSalir;

    // Rectángulos de botones - Submenú Opciones
    QRect btnCreditos;
    QRect btnDificultad;
    QRect btnNivel1;
    QRect btnNivel2;
    QRect btnNivel3;
    QRect btnVolver;

    // Rectángulos de botones - Dificultad
    QRect btnFacil;
    QRect btnNormal;
    QRect btnDificil;
    QRect btnVolverDificultad;

    // Estados de hover - Menú Principal
    bool hoverInicio = false;
    bool hoverOpciones = false;
    bool hoverSalir = false;

    // Estados de hover - Submenú Opciones
    bool hoverCreditos = false;
    bool hoverDificultad = false;
    bool hoverNivel1 = false;
    bool hoverNivel2 = false;
    bool hoverNivel3 = false;
    bool hoverVolver = false;

    // Estados de hover - Dificultad
    bool hoverFacil = false;
    bool hoverNormal = false;
    bool hoverDificil = false;
    bool hoverVolverDificultad = false;

    // Métodos de carga
    void cargarRecursos();
    void cargarSpritesButtons();
    void definirRectangulos();

    // Métodos de dibujo
    void dibujarMenuPrincipal(QPainter &painter);
    void dibujarMenuOpciones(QPainter &painter);
    void dibujarMenuCreditos(QPainter &painter);
    void dibujarMenuDificultad(QPainter &painter);
    void dibujarBoton(QPainter &painter, const QRect &rect, const QString &texto,
                      const QPixmap &normal, const QPixmap &hover, bool isHover,
                      const QColor &colorBase = QColor(60, 60, 100));
    void dibujarBotonNivel(QPainter &painter, const QRect &rect, const QString &titulo,
                           const QString &subtitulo, const QColor &color, bool hover);
    void dibujarTitulo(QPainter &painter);
    void dibujarBotonSonido(QPainter &painter);

    // Verificación de hover
    void verificarHoverPrincipal(int x, int y);
    void verificarHoverOpciones(int x, int y);
    void verificarHoverCreditos(int x, int y);
    void verificarHoverDificultad(int x, int y);

signals:
    void iniciarNivel(int nivel);
    void mostrarCreditos();
    void salirJuego();
    void cambiarDificultad(int dificultad);

private slots:
    void onInicioClicked();
    void onOpcionesClicked();
    void onSalirClicked();
    void onCreditosClicked();
    void onDificultadClicked();
    void onNivel1Clicked();
    void onNivel2Clicked();
    void onNivel3Clicked();
    void onVolverClicked();
    void onFacilClicked();
    void onNormalClicked();
    void onDificilClicked();
    void onSoundToggleClicked();
};

#endif // MENU_H
