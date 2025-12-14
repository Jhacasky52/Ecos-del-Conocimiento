#include "GameManager.h"
#include <QDebug>

GameManager::GameManager(QObject *parent)
    : QObject(parent),
      estadoActual(MENU),
      nivelActual(1)
{
    qDebug() << "[GameManager] Inicializado";
}

void GameManager::iniciarNuevoJuego()
{
    nivelActual = 1;
    estadoActual = JUGANDO;
    emit estadoCambiado(estadoActual);
    emit nivelCambiado(nivelActual);
    qDebug() << "[GameManager] Nuevo juego iniciado";
}

void GameManager::pausar()
{
    if (estadoActual == JUGANDO) {
        estadoActual = PAUSA;
        emit estadoCambiado(estadoActual);
        qDebug() << "[GameManager] Juego pausado";
    }
}

void GameManager::reanudar()
{
    if (estadoActual == PAUSA) {
        estadoActual = JUGANDO;
        emit estadoCambiado(estadoActual);
        qDebug() << "[GameManager] Juego reanudado";
    }
}

void GameManager::terminarJuego()
{
    estadoActual = MENU;
    emit estadoCambiado(estadoActual);
    qDebug() << "[GameManager] Juego terminado";
}
