#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMap>
#include <QString>
#include <QDebug>

/**
 * Gestor de sonido del juego (Singleton)
 * 
 * Maneja:
 * - Música de fondo por nivel
 * - Efectos de sonido
 * - Control de volumen global
 * - Toggle de sonido on/off
 * 
 * Sonidos disponibles:
 * - menu_principal: Música del menú
 * - nivel1_background: Música del Nivel 1
 * - nivel1_prensa: Efecto al accionar la imprenta
 * - nivel2_background: Música del Nivel 2
 * - nivel3_background: Música del Nivel 3 (a agregar)
 * - victoria: Música de victoria
 * - derrota: Música de derrota
 */
class SoundManager : public QObject
{
    Q_OBJECT

public:
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }

    // =========================================================================
    // CONTROL DE MÚSICA DE FONDO
    // =========================================================================
    
    /**
     * Reproduce la música de fondo del menú principal
     */
    void playMenuMusic() {
        playBackgroundMusic("menu_principal");
    }
    
    /**
     * Reproduce la música de fondo de un nivel específico
     * @param nivel Número del nivel (1, 2 o 3)
     */
    void playLevelMusic(int nivel) {
        QString key;
        switch(nivel) {
            case 1: key = "nivel1_background"; break;
            case 2: key = "nivel2_background"; break;
            case 3: key = "nivel3_background"; break;
            default: key = "menu_principal"; break;
        }
        playBackgroundMusic(key);
    }
    
    /**
     * Reproduce música de victoria
     */
    void playVictoryMusic() {
        stopBackgroundMusic();
        playBackgroundMusic("victoria");
    }
    
    /**
     * Reproduce música de derrota
     */
    void playDefeatMusic() {
        stopBackgroundMusic();
        playBackgroundMusic("derrota");
    }
    
    /**
     * Detiene la música de fondo actual
     */
    void stopBackgroundMusic() {
        if (musicPlayer) {
            musicPlayer->stop();
            currentMusic = "";
        }
    }
    
    /**
     * Pausa la música de fondo actual
     */
    void pauseBackgroundMusic() {
        if (musicPlayer && musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            musicPlayer->pause();
        }
    }
    
    /**
     * Reanuda la música de fondo pausada
     */
    void resumeBackgroundMusic() {
        if (musicPlayer && musicPlayer->playbackState() == QMediaPlayer::PausedState) {
            musicPlayer->play();
        }
    }

    // =========================================================================
    // EFECTOS DE SONIDO
    // =========================================================================
    
    /**
     * Reproduce el efecto de la prensa (Nivel 1)
     */
    void playSoundPrensa() {
        playSoundEffect("nivel1_prensa");
    }
    
    /**
     * Reproduce un efecto de sonido específico
     * @param soundKey Clave del sonido a reproducir
     */
    void playSoundEffect(const QString &soundKey) {
        if (!soundEnabled || !effectPlayer) return;
        
        QString path = getSoundPath(soundKey);
        if (path.isEmpty()) {
            qWarning() << "[SoundManager] Efecto no encontrado:" << soundKey;
            return;
        }
        
        effectPlayer->setSource(QUrl::fromLocalFile(path));
        effectPlayer->play();
        qDebug() << "[SoundManager] Reproduciendo efecto:" << soundKey;
    }

    // =========================================================================
    // CONTROL GENERAL
    // =========================================================================
    
    /**
     * Activa o desactiva todo el sonido
     */
    void setSoundEnabled(bool enabled) {
        soundEnabled = enabled;
        if (musicPlayer) {
            if (enabled) {
                if (!currentMusic.isEmpty()) {
                    musicPlayer->play();
                }
            } else {
                musicPlayer->pause();
            }
        }
        qDebug() << "[SoundManager] Sonido" << (enabled ? "activado" : "desactivado");
        emit soundStateChanged(enabled);
    }
    
    /**
     * Toggle del estado del sonido
     */
    void toggleSound() {
        setSoundEnabled(!soundEnabled);
    }
    
    /**
     * Verifica si el sonido está habilitado
     */
    bool isSoundEnabled() const {
        return soundEnabled;
    }
    
    /**
     * Establece el volumen de la música (0.0 - 1.0)
     */
    void setMusicVolume(float volume) {
        musicVolume = qBound(0.0f, volume, 1.0f);
        if (musicOutput) {
            musicOutput->setVolume(musicVolume);
        }
    }
    
    /**
     * Establece el volumen de los efectos (0.0 - 1.0)
     */
    void setEffectsVolume(float volume) {
        effectsVolume = qBound(0.0f, volume, 1.0f);
        if (effectOutput) {
            effectOutput->setVolume(effectsVolume);
        }
    }
    
    /**
     * Obtiene el volumen actual de la música
     */
    float getMusicVolume() const { return musicVolume; }
    
    /**
     * Obtiene el volumen actual de los efectos
     */
    float getEffectsVolume() const { return effectsVolume; }

signals:
    void soundStateChanged(bool enabled);

private:
    SoundManager() : QObject(nullptr),
        musicPlayer(nullptr),
        effectPlayer(nullptr),
        musicOutput(nullptr),
        effectOutput(nullptr),
        soundEnabled(true),
        musicVolume(0.7f),
        effectsVolume(1.0f)
    {
        initializePlayers();
        initializeSoundPaths();
        qDebug() << "[SoundManager] Inicializado";
    }
    
    ~SoundManager() {
        if (musicPlayer) { musicPlayer->stop(); delete musicPlayer; }
        if (effectPlayer) { effectPlayer->stop(); delete effectPlayer; }
        if (musicOutput) delete musicOutput;
        if (effectOutput) delete effectOutput;
    }
    
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    
    void initializePlayers() {
        // Reproductor de música de fondo
        musicPlayer = new QMediaPlayer(this);
        musicOutput = new QAudioOutput(this);
        musicPlayer->setAudioOutput(musicOutput);
        musicOutput->setVolume(musicVolume);
        
        // Conectar para loop de música
        connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia && soundEnabled) {
                // Loop de la música de fondo (excepto victoria/derrota)
                if (currentMusic != "victoria" && currentMusic != "derrota") {
                    musicPlayer->setPosition(0);
                    musicPlayer->play();
                }
            }
        });
        
        // Reproductor de efectos de sonido
        effectPlayer = new QMediaPlayer(this);
        effectOutput = new QAudioOutput(this);
        effectPlayer->setAudioOutput(effectOutput);
        effectOutput->setVolume(effectsVolume);
    }
    
    void initializeSoundPaths() {
        // Rutas de los archivos de sonido
        QString basePath = ":/assets/sounds/";
        
        soundPaths["menu_principal"] = basePath + "menu_principal.wav";
        soundPaths["nivel1_background"] = basePath + "nivel1_background.mp3";
        soundPaths["nivel1_prensa"] = basePath + "nivel1_prensa.wav";
        soundPaths["nivel2_background"] = basePath + "nivel2_background.wav";
        soundPaths["nivel3_background"] = basePath + "nivel3_background.wav";
        soundPaths["victoria"] = basePath + "victoria.mp3";
        soundPaths["derrota"] = basePath + "derrota.wav";
    }
    
    QString getSoundPath(const QString &key) const {
        return soundPaths.value(key, "");
    }
    
    void playBackgroundMusic(const QString &key) {
        if (!soundEnabled || !musicPlayer) return;
        
        // No reiniciar si ya está reproduciendo la misma música
        if (currentMusic == key && musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            return;
        }
        
        QString path = getSoundPath(key);
        if (path.isEmpty()) {
            qWarning() << "[SoundManager] Música no encontrada:" << key;
            return;
        }
        
        currentMusic = key;
        musicPlayer->setSource(QUrl(path));
        musicPlayer->play();
        qDebug() << "[SoundManager] Reproduciendo música:" << key;
    }

    QMediaPlayer *musicPlayer;
    QMediaPlayer *effectPlayer;
    QAudioOutput *musicOutput;
    QAudioOutput *effectOutput;
    
    QMap<QString, QString> soundPaths;
    QString currentMusic;
    
    bool soundEnabled;
    float musicVolume;
    float effectsVolume;
};

#endif // SOUNDMANAGER_H
