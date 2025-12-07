#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow ventana;
    ventana.setWindowTitle("Ecos del Conocimiento - Nivel 1");
    ventana.setFixedSize(1024, 768);
    ventana.show();

    return app.exec();
}
