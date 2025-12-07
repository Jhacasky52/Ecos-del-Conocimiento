#include "Nivel1.h"
#include "ResourceManager.h"
#include <QDebug>

Nivel1::Nivel1(QObject *parent)
    : QObject(parent),
    jugador(nullptr)
{
    qDebug() << "\n========== [Nivel1] Inicializando ==========";

    // PASO 1: Cargar recursos ANTES de crear jugador
    ResourceManager::getInstance().cargarRecursosNivel1();

    // PASO 2: Crear jugador (ahora que los recursos están cargados)
    jugador = new Jugador(this);
    jugador->setLimites(1024.0f, 768.0f);

    // PASO 3: Cargar fondo
    fondoOriginal = ResourceManager::getInstance().getSprite("fondo_nivel1");
    if (!fondoOriginal.isNull()) {
        fondoEscalado = fondoOriginal.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        qDebug() << "[Nivel1] Fondo cargado exitosamente:" << fondoEscalado.size();
    } else {
        qWarning() << "[Nivel1] No se pudo cargar el fondo_nivel1";
    }

    qDebug() << "========== [Nivel1] Inicializado ==========\n";
}

Nivel1::~Nivel1()
{
    qDebug() << "[Nivel1] Destruido";
}

void Nivel1::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    if (jugador) {
        jugador->actualizar(arriba, abajo, izquierda, derecha, deltaTime);
    }
}

void Nivel1::dibujar(QPainter &painter)
{
    dibujarFondo(painter);

    if (jugador) {
        jugador->dibujar(painter);
    }

    dibujarUI(painter);
}

void Nivel1::dibujarFondo(QPainter &painter)
{
    if (!fondoEscalado.isNull()) {
        painter.drawPixmap(0, 0, fondoEscalado);
    } else {
        // Fallback: color sólido si no carga el fondo
        painter.fillRect(0, 0, 1024, 768, QColor(139, 90, 43));
    }
}

void Nivel1::dibujarUI(QPainter &painter)
{
    painter.setPen(Qt::white);

    QFont fontTitulo;
    fontTitulo.setPointSize(24);
    fontTitulo.setBold(true);
    painter.setFont(fontTitulo);
    painter.drawText(200, 30, "El Taller de Gutenberg - Nivel 1");

    QFont fontInstrucciones;
    fontInstrucciones.setPointSize(12);
    painter.setFont(fontInstrucciones);
    painter.drawText(10, 750, "WASD o Flechas para mover | ESC para salir");
}
