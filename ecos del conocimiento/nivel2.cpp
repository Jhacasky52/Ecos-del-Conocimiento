#include "Nivel2.h"
#include "ResourceManager.h"
#include <QDebug>

Nivel2::Nivel2(QObject *parent)
    : QObject(parent),
    jugador(nullptr)
{
    qDebug() << "\n========== [Nivel2] Inicializando ==========";

    // PASO 1: Cargar recursos ANTES de crear jugador
    ResourceManager::getInstance().cargarRecursosNivel2();

    // PASO 2: Crear jugador (ahora que los recursos están cargados)
    jugador = new Jugador(this);
    jugador->setLimites(1024.0f, 768.0f);
    jugador->setPosicion(100.0f, 600.0f); // Posición inicial diferente al nivel 1

    // PASO 3: Cargar fondo
    fondoOriginal = ResourceManager::getInstance().getSprite("fondo_nivel2");
    if (!fondoOriginal.isNull()) {
        fondoEscalado = fondoOriginal.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        qDebug() << "[Nivel2] Fondo cargado exitosamente:" << fondoEscalado.size();
    } else {
        qWarning() << "[Nivel2] No se pudo cargar el fondo_nivel2";
    }

    qDebug() << "========== [Nivel2] Inicializado ==========\n";
}

Nivel2::~Nivel2()
{
    qDebug() << "[Nivel2] Destruido";
}

void Nivel2::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    if (jugador) {
        jugador->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
    }
}

void Nivel2::dibujar(QPainter &painter)
{
    dibujarFondo(painter);

    if (jugador) {
        jugador->dibujar(painter);
    }

    dibujarUI(painter);
}

void Nivel2::dibujarFondo(QPainter &painter)
{
    if (!fondoEscalado.isNull()) {
        painter.drawPixmap(0, 0, fondoEscalado);
    } else {
        // Fallback: color sólido si no carga el fondo
        painter.fillRect(0, 0, 1024, 768, QColor(40, 30, 20));
    }
}

void Nivel2::dibujarUI(QPainter &painter)
{
    painter.setPen(Qt::white);

    QFont fontTitulo;
    fontTitulo.setPointSize(24);
    fontTitulo.setBold(true);
    painter.setFont(fontTitulo);
    painter.drawText(150, 30, "La Biblioteca de Alejandría - Nivel 2");

    QFont fontInstrucciones;
    fontInstrucciones.setPointSize(12);
    painter.setFont(fontInstrucciones);
    painter.drawText(10, 750, "WASD o Flechas para mover | ESC para salir");
}
