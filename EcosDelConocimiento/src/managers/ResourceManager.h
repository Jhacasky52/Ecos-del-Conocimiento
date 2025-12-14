#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>
#include <QDebug>
#include <stdexcept>

/**
 * @brief Gestor de recursos del juego (Singleton).
 * 
 * Implementa:
 * - Patrón Singleton
 * - Manejo de excepciones (requisito del proyecto)
 * - Caché de sprites
 */
class ResourceManager {
public:
    /**
     * @brief Obtiene la instancia única del ResourceManager
     */
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    /**
     * @brief Carga los recursos del menú principal
     */
    void cargarRecursosMenu() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Menú ==========";
        cargarSprite("menu_fondo", ":/assets/fondos/menu.png");
        qDebug() << "========== [ResourceManager] Recursos Menú cargados ==========\n";
    }

    /**
     * @brief Carga los recursos del Nivel 1
     */
    void cargarRecursosNivel1() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Nivel 1 ==========";

        // Fondo
        cargarSprite("fondo_nivel1", ":/assets/fondos/nivel1_taller_gutenberg.png");

        // Jugador: 4 direcciones
        cargarSprite("nivel1_jugador_frente", ":/assets/sprites/nivel1_automata_jugador_frente.png");
        cargarSprite("nivel1_jugador_espalda", ":/assets/sprites/nivel1_automata_jugador_espalda.png");
        cargarSprite("nivel1_jugador_izquierda", ":/assets/sprites/nivel1_automata_jugador_izquierda.png");
        cargarSprite("nivel1_jugador_derecha", ":/assets/sprites/nivel1_automata_jugador_derecha.png");
        
        // Mesas
        cargarSprite("nivel1_mesa_arriba", ":/assets/sprites/nivel1_mesa_arriba.png");
        cargarSprite("nivel1_mesa_abajo", ":/assets/sprites/nivel1_mesa_abajo.png");
        
        // Letras
        cargarSprite("nivel1_letras", ":/assets/sprites/nivel1_letras.png");

        // Imprenta
        cargarSprite("nivel1_prensa_imprenta", ":/assets/sprites/nivel1_prensa_imprenta.png");
        cargarSprite("nivel1_prensa_imprenta2", ":/assets/sprites/nivel1_prensa_imprenta2.png");
        
        qDebug() << "========== [ResourceManager] Recursos Nivel 1 cargados ==========\n";
    }

    /**
     * @brief Obtiene un sprite por su ID
     * @param id Identificador del sprite
     * @return QPixmap del sprite (puede ser null si no existe)
     */
    QPixmap getSprite(const QString &id) const {
        auto it = sprites.find(id);
        if (it == sprites.end()) {
            qWarning() << "[ResourceManager] !!! Sprite NO encontrado para id:" << id;
            return QPixmap();
        }
        return it.value();
    }
    
    /**
     * @brief Verifica si un sprite existe
     */
    bool existeSprite(const QString &id) const {
        return sprites.contains(id) && !sprites[id].isNull();
    }
    
    /**
     * @brief Limpia todos los recursos cargados
     */
    void limpiarRecursos() {
        sprites.clear();
        qDebug() << "[ResourceManager] Recursos limpiados";
    }

private:
    ResourceManager() {}
    ~ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    QMap<QString, QPixmap> sprites;

    /**
     * @brief Carga un sprite con manejo de excepciones
     * @throws std::runtime_error si no se puede cargar el recurso crítico
     */
    void cargarSprite(const QString &id, const QString &ruta) {
        try {
            QPixmap img(ruta);
            
            if (img.isNull()) {
                // Lanzar excepción para recursos críticos
                throw std::runtime_error(
                    QString("No se pudo cargar el recurso: %1 desde %2")
                        .arg(id, ruta).toStdString()
                );
            }
            
            sprites.insert(id, img);
            qDebug() << "[ResourceManager] OK" << id << "desde" << ruta
                     << "| Tamaño:" << img.width() << "x" << img.height();
                     
        } catch (const std::runtime_error& e) {
            qWarning() << "[ResourceManager] EXCEPCIÓN:" << e.what();
            
            // Crear imagen de fallback (magenta para identificar errores)
            QPixmap fallback(64, 64);
            fallback.fill(Qt::magenta);
            sprites.insert(id, fallback);
            
            qWarning() << "[ResourceManager] Usando imagen fallback para:" << id;
            
        } catch (const std::exception& e) {
            qCritical() << "[ResourceManager] Error inesperado:" << e.what();
            sprites.insert(id, QPixmap());
        }
    }
};

#endif // RESOURCEMANAGER_H
