#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>

/**
 * @brief Menú principal del juego.
 * 
 * Muestra:
 * - Botón Jugar
 * - Botón Créditos
 * - Botón Salir
 */
class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPixmap fondoMenu;

    // Rectángulos de botones
    QRect btnJugar;
    QRect btnCreditos;
    QRect btnSalir;

    // Estado de hover
    bool hoverjugar = false;
    bool hoverCreditos = false;
    bool hoverSalir = false;

    void cargarRecursos();
    void dibujarBotones(QPainter &painter);
    void verificarHover(int x, int y);

signals:
    void iniciarJuego();
    void mostrarCreditos();
    void salirJuego();

private slots:
    void onJugarClicked();
    void onCreditosClicked();
    void onSalirClicked();
};

#endif // MENU_H
