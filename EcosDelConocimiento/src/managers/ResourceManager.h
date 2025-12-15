#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QMap>
#include <QString>
#include <QDebug>
#include <stdexcept>

// Gestor de recursos del juego (Singleton)
// Implementa: Patrón Singleton, Manejo de excepciones, Caché de sprites

class ResourceManager {
public:
    // Obtiene la instancia única del ResourceManager
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    // Carga los recursos del menú principal
    void cargarRecursosMenu() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Menú ==========";
        cargarSprite("menu_fondo", ":/assets/fondos/menu.png");
        cargarSprite("menu_opciones", ":/assets/fondos/menu_opciones.jpg");
        cargarSprite("btn_jugar_normal", ":/assets/fondos/jugar_normal.png");
        cargarSprite("btn_jugar_hover", ":/assets/fondos/jugar_hover.png");
        cargarSprite("btn_creditos_normal", ":/assets/fondos/creditos_normal.png");
        cargarSprite("btn_creditos_hover", ":/assets/fondos/creditos_hover.png");
        cargarSprite("btn_salir_normal", ":/assets/fondos/salir_normal.png");
        cargarSprite("btn_salir_hover", ":/assets/fondos/salir_hover.png");
        cargarSprite("btn_opciones_normal", ":/assets/fondos/opciones_normal.png");
        cargarSprite("btn_opciones_hover", ":/assets/fondos/opciones_hover.png");
        
        // Fondos de pantallas de estado
        cargarSprite("fondo_victoria", ":/assets/fondos/victoria.png");
        cargarSprite("fondo_derrota", ":/assets/fondos/derrota.png");
        
        qDebug() << "========== [ResourceManager] Recursos Menú cargados ==========\n";
    }
    
    // Carga los recursos del Nivel 1
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

    // Carga los recursos del Nivel 2 - Biblioteca en Llamas
    // Incluye 3 capas de parallax y todos los sprites
    void cargarRecursosNivel2() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Nivel 2 ==========";

        // Fondos parallax (3 capas)
        cargarSprite("nivel2_fondo_lejano", ":/assets/fondos/nivel2_lejano.png");
        cargarSprite("nivel2_fondo_medio", ":/assets/fondos/nivel2_medio.png");
        cargarSprite("nivel2_fondo_cerca", ":/assets/fondos/nivel2_cerca.png");
        
        // Fondo completo (fallback)
        cargarSprite("fondo_nivel2", ":/assets/fondos/nivel2_biblioteca_alejandria.png");

        // Jugador nivel 2 (sprite sheets 612x408, 2 frames cada uno)
        cargarSprite("nivel2_jugador_derecha", ":/assets/sprites/nivel2_jugador_derecha.png");
        cargarSprite("nivel2_jugador_izquierda", ":/assets/sprites/nivel2_jugador_izquierda.png");
        cargarSprite("nivel2_salto_derecha", ":/assets/sprites/nivel2_jugador_salto_derecha.png");
        cargarSprite("nivel2_salto_izquierda", ":/assets/sprites/nivel2_jugador_salto_izquierda.png");
        
        // Pergaminos (3 estados)
        cargarSprite("nivel2_pergamino", ":/assets/sprites/nivel2_pergamino.png");
        cargarSprite("nivel2_pergamino_pocofuego", ":/assets/sprites/nivel2_pergamino_pocofuego.png");
        cargarSprite("nivel2_pergamino_muchofuego", ":/assets/sprites/nivel2_pergamino_muchofuego.png");
        
        // Fuego
        cargarSprite("nivel2_fuego_aire", ":/assets/sprites/nivel2_fuego_aire.png");
        cargarSprite("nivel2_fuego_piso", ":/assets/sprites/nivel2_fuego_piso.png");
        
        // Objetos interactivos
        cargarSprite("nivel2_fuente", ":/assets/sprites/nivel2_fuente.png");
        cargarSprite("nivel2_cofre", ":/assets/sprites/nivel2_cofrepergaminos.png");
        cargarSprite("nivel2_cubeta", ":/assets/sprites/nivel2_cubeta_agua.png");
        
        // Cuervo (4 sprites de animación)
        cargarSprite("nivel2_cuervo_derecha", ":/assets/sprites/nivel2_cuervo_derecha.png");
        cargarSprite("nivel2_cuervo_derecha2", ":/assets/sprites/nivel2_cuervo_derecha2.png");
        cargarSprite("nivel2_cuervo_izquierda", ":/assets/sprites/nivel2_cuervo_izquierda.png");
        cargarSprite("nivel2_cuervo_izquierda2", ":/assets/sprites/nivel2_cuervo_izquierda2.png");
        
        qDebug() << "========== [ResourceManager] Recursos Nivel 2 cargados ==========\n";
    }

    // Carga los recursos del Nivel 3 - Revolución Digital
    void cargarRecursosNivel3() {
        qDebug() << "\n========== [ResourceManager] Cargando recursos Nivel 3 ==========";

        // Fondo único
        cargarSprite("fondo_nivel3", ":/assets/fondos/nivel3_revolucion_digital.png");

        // Jugador nivel 3 (Packet)
        cargarSprite("nivel3_packet", ":/assets/sprites/nivel3_packet.png");
        cargarSprite("nivel3_packet_escudo", ":/assets/sprites/nivel3_packet_escudo.png");
        cargarSprite("nivel3_jugador_frente", ":/assets/sprites/nivel3_packet.png");
        cargarSprite("nivel3_jugador_espalda", ":/assets/sprites/nivel3_packet.png");
        cargarSprite("nivel3_jugador_izquierda", ":/assets/sprites/nivel3_packet.png");
        cargarSprite("nivel3_jugador_derecha", ":/assets/sprites/nivel3_packet.png");
        
        // Objetos del nivel 3
        cargarSprite("nivel3_fragmento", ":/assets/sprites/nivel3_fragmento.png");
        cargarSprite("nivel3_datos", ":/assets/sprites/nivel3_fragmento.png");
        cargarSprite("nivel3_virus", ":/assets/sprites/nivel3_virus.png");
        cargarSprite("nivel3_nodo", ":/assets/sprites/nivel3_nodo.png");
        cargarSprite("nivel3_firewall", ":/assets/sprites/nivel3_firewall.png");
        cargarSprite("nivel3_terminal", ":/assets/sprites/nivel3_terminal.png");
        cargarSprite("nivel3_guardian", ":/assets/sprites/nivel3_guardian.png");
        
        qDebug() << "========== [ResourceManager] Recursos Nivel 3 cargados ==========\n";
    }

    // Obtiene un sprite por su ID (puede ser null si no existe)
    QPixmap getSprite(const QString &id) const {
        auto it = sprites.find(id);
        if (it == sprites.end()) {
            qWarning() << "[ResourceManager] !!! Sprite NO encontrado para id:" << id;
            return QPixmap();
        }
        return it.value();
    }
    
    // Verifica si un sprite existe
    bool existeSprite(const QString &id) const {
        return sprites.contains(id) && !sprites[id].isNull();
    }
    
    // Limpia todos los recursos cargados
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

    // Carga un sprite con manejo de excepciones
    // Lanza std::runtime_error si no se puede cargar el recurso crítico
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
