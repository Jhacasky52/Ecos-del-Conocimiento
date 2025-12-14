#include <QApplication>
#include "Menu.h"
#include "MainWindow.h"
#include "ResourceManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Crear menú principal
    Menu *menu = new Menu();
    menu->show();

    // Conectar señal del menú para iniciar el juego
    QObject::connect(menu, &Menu::iniciarJuego, [&app]() {
        qDebug() << "[Main] Cargando recursos del Nivel 1...";

        // Cargar recursos del nivel 1
        ResourceManager::getInstance().cargarRecursosNivel1();

        // Crear ventana del juego
        MainWindow *juego = new MainWindow();
        juego->show();

        qDebug() << "[Main] Juego iniciado correctamente";
    });

    // Si cierra el menú sin jugar, cerrar la aplicación
    QObject::connect(menu, &QWidget::destroyed, &app, &QApplication::quit);

    return app.exec();
}
