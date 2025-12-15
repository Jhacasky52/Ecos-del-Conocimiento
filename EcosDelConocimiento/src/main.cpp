#include <QApplication>
#include "Menu.h"
#include "MainWindow.h"
#include "ResourceManager.h"

// Declaración forward para las funciones
void mostrarMenu();
void iniciarJuego(int nivel);

// Variables globales para el ciclo de vida de la aplicación
QApplication *app = nullptr;
Menu *menuActual = nullptr;
MainWindow *juegoActual = nullptr;

void limpiarJuego()
{
    if (juegoActual) {
        juegoActual->deleteLater();
        juegoActual = nullptr;
    }
}

void limpiarMenu()
{
    if (menuActual) {
        menuActual->deleteLater();
        menuActual = nullptr;
    }
}

void mostrarMenu()
{
    limpiarJuego();
    limpiarMenu();
    
    // Crear nuevo menú
    menuActual = new Menu();
    
    // Conectar señal para iniciar un nivel
    QObject::connect(menuActual, &Menu::iniciarNivel, [](int nivel) {
        iniciarJuego(nivel);
    });
    
    // Conectar señal de salir
    QObject::connect(menuActual, &Menu::salirJuego, []() {
        if (app) app->quit();
    });
    
    menuActual->show();
    qDebug() << "[Main] Menú principal mostrado";
}

void iniciarJuego(int nivel)
{
    qDebug() << "[Main] Cargando recursos del Nivel" << nivel << "...";
    
    // Cargar recursos según el nivel (cargar todos para el flujo entre niveles)
    ResourceManager::getInstance().cargarRecursosNivel1();
    ResourceManager::getInstance().cargarRecursosNivel2();
    ResourceManager::getInstance().cargarRecursosNivel3();
    
    limpiarMenu();
    limpiarJuego();
    
    // Crear ventana del juego con el nivel seleccionado
    juegoActual = new MainWindow(nivel);
    
    // Conectar señal para volver al menú
    QObject::connect(juegoActual, &MainWindow::volverAlMenu, []() {
        mostrarMenu();
    });
    
    // Cuando se destruya la ventana del juego sin emitir volverAlMenu
    QObject::connect(juegoActual, &QWidget::destroyed, []() {
        // Solo mostrar menú si no hay uno activo
        if (!menuActual || !menuActual->isVisible()) {
            // No hacer nada, ya se manejó con volverAlMenu o el usuario cerró
        }
    });
    
    juegoActual->show();
    qDebug() << "[Main] Nivel" << nivel << "iniciado correctamente";
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    app = &application;
    
    // Estilo global de la aplicación
    application.setStyle("fusion");
    
    // Mostrar menú inicial
    mostrarMenu();

    return application.exec();
}
