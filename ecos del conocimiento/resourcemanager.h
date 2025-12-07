#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>
#include <QDebug>

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    void cargarRecursosNivel1() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Nivel 1 ==========";

        // Fondo
        cargarSprite("fondo_nivel1", ":/assets/fondos/nivel1_taller_gutenberg.png");

        // Jugador: 4 direcciones
        cargarSprite("nivel1_jugador_frente", ":/assets/sprites/nivel1_automata_jugador_frente.png");
        cargarSprite("nivel1_jugador_espalda", ":/assets/sprites/nivel1_automata_jugador_espalda.png");
        cargarSprite("nivel1_jugador_izquierda", ":/assets/sprites/nivel1_automata_jugador_izquierda.png");
        cargarSprite("nivel1_jugador_derecha", ":/assets/sprites/nivel1_automata_jugador_derecha.png");

        qDebug() << "========== [ResourceManager] Recursos Nivel 1 cargados ==========\n";
    }

    void cargarRecursosNivel2() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Nivel 2 ==========";

        // Fondo
        cargarSprite("fondo_nivel2", ":/assets/fondos/nivel2_biblioteca_alejandria.png");

        // Jugador: 4 direcciones (reutilizamos los sprites del nivel 1)
        cargarSprite("nivel1_jugador_frente", ":/assets/sprites/nivel1_automata_jugador_frente.png");
        cargarSprite("nivel1_jugador_espalda", ":/assets/sprites/nivel1_automata_jugador_espalda.png");
        cargarSprite("nivel1_jugador_izquierda", ":/assets/sprites/nivel1_automata_jugador_izquierda.png");
        cargarSprite("nivel1_jugador_derecha", ":/assets/sprites/nivel1_automata_jugador_derecha.png");

        qDebug() << "========== [ResourceManager] Recursos Nivel 2 cargados ==========\n";
    }

    QPixmap getSprite(const QString &id) const {
        auto it = sprites.find(id);
        if (it == sprites.end()) {
            qWarning() << "[ResourceManager] !!! Sprite NO encontrado para id:" << id;
            return QPixmap();
        }
        return it.value();
    }

private:
    ResourceManager() {}
    ~ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    QMap<QString, QPixmap> sprites;

    void cargarSprite(const QString &id, const QString &ruta) {
        QPixmap img(ruta);
        if (img.isNull()) {
            qWarning() << "[ResourceManager] X FALLO cargando" << id << "desde" << ruta;
        } else {
            qDebug() << "[ResourceManager] OK" << id << "desde" << ruta
                     << "| Tamaño:" << img.width() << "x" << img.height();
        }
        sprites.insert(id, img);
    }
};

#endif // RESOURCEMANAGER_H
