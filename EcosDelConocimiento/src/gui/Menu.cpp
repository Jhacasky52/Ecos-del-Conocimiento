#include "Menu.h"
#include "ResourceManager.h"
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QScreen>

Menu::Menu(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Ecos del Conocimiento - Menú Principal");

    // Configurar tamaño fijo sin opción de maximizar
    setFixedSize(1024, 768);

    // Centrar la ventana en la pantalla
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - 1024) / 2;
    int y = (screenGeometry.height() - 768) / 2;
    move(x, y);

    // Cargar recursos del menú
    cargarRecursos();

    // Definir posiciones de botones
    btnJugar = QRect(350, 350, 324, 60);
    btnCreditos = QRect(350, 450, 324, 60);
    btnSalir = QRect(350, 550, 324, 60);

    setMouseTracking(true);

    qDebug() << "[Menu] Menú inicializado correctamente en posición" << x << "," << y;
}

Menu::~Menu()
{
}

void Menu::cargarRecursos()
{
    // Cargar recursos del menú desde ResourceManager
    ResourceManager::getInstance().cargarRecursosMenu();

    // Obtener sprite del menú
    fondoMenu = ResourceManager::getInstance().getSprite("menu_fondo");

    if (fondoMenu.isNull()) {
        qWarning() << "[Menu] No se pudo cargar la imagen de fondo del menú desde ResourceManager";
        fondoMenu = QPixmap(1024, 768);
        fondoMenu.fill(Qt::black);
    } else {
        qDebug() << "[Menu] Fondo cargado correctamente desde ResourceManager";
        fondoMenu = fondoMenu.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

void Menu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // Dibujar fondo
    painter.drawPixmap(0, 0, fondoMenu);

    // Dibujar botones
    dibujarBotones(painter);
}

void Menu::dibujarBotones(QPainter &painter)
{
    // Botón JUGAR
    if (hoverjugar) {
        painter.fillRect(btnJugar, QColor(255, 150, 0, 200)); // Naranja hover
        painter.setPen(QPen(QColor(255, 200, 50), 3));
    } else {
        painter.fillRect(btnJugar, QColor(200, 100, 0, 180)); // Naranja normal
        painter.setPen(QPen(QColor(255, 150, 0), 2));
    }
    painter.drawRect(btnJugar);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(btnJugar, Qt::AlignCenter, "JUGAR");

    // Botón CRÉDITOS
    if (hoverCreditos) {
        painter.fillRect(btnCreditos, QColor(100, 150, 255, 200)); // Azul hover
        painter.setPen(QPen(QColor(150, 200, 255), 3));
    } else {
        painter.fillRect(btnCreditos, QColor(50, 100, 200, 180)); // Azul normal
        painter.setPen(QPen(QColor(100, 150, 255), 2));
    }
    painter.drawRect(btnCreditos);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(btnCreditos, Qt::AlignCenter, "CRÉDITOS");

    // Botón SALIR
    if (hoverSalir) {
        painter.fillRect(btnSalir, QColor(255, 100, 100, 200)); // Rojo hover
        painter.setPen(QPen(QColor(255, 150, 150), 3));
    } else {
        painter.fillRect(btnSalir, QColor(200, 50, 50, 180)); // Rojo normal
        painter.setPen(QPen(QColor(255, 100, 100), 2));
    }
    painter.drawRect(btnSalir);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(btnSalir, Qt::AlignCenter, "SALIR");
}

void Menu::mouseMoveEvent(QMouseEvent *event)
{
    verificarHover(event->position().x(), event->position().y());
    update();
}

void Menu::verificarHover(int x, int y)
{
    hoverjugar = btnJugar.contains(x, y);
    hoverCreditos = btnCreditos.contains(x, y);
    hoverSalir = btnSalir.contains(x, y);

    if (hoverjugar || hoverCreditos || hoverSalir) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void Menu::mousePressEvent(QMouseEvent *event)
{
    int x = event->position().x();
    int y = event->position().y();

    if (btnJugar.contains(x, y)) {
        onJugarClicked();
    } else if (btnCreditos.contains(x, y)) {
        onCreditosClicked();
    } else if (btnSalir.contains(x, y)) {
        onSalirClicked();
    }
}

void Menu::onJugarClicked()
{
    qDebug() << "[Menu] Iniciar juego - Cerrando menú";
    emit iniciarJuego();
    close();
}

void Menu::onCreditosClicked()
{
    qDebug() << "[Menu] Mostrar créditos";
    emit mostrarCreditos();
}

void Menu::onSalirClicked()
{
    qDebug() << "[Menu] Salir del juego";
    QApplication::quit();
}
