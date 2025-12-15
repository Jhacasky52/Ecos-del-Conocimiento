#include "Menu.h"
#include "ResourceManager.h"
#include "DifficultyManager.h"
#include "SoundManager.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QLinearGradient>

Menu::Menu(QWidget *parent)
    : QWidget(parent), estadoActual(MENU_PRINCIPAL)
{
    setWindowTitle("Ecos del Conocimiento - Menú Principal");

    // Configurar tamaño fijo sin opción de maximizar
    setFixedSize(1024, 768);

    // Centrar la ventana en la pantalla
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - 1024) / 2;
    int y = (screenGeometry.height() - 768) / 2;
    move(x, y);

    // Cargar recursos del menú
    cargarRecursos();

    // Cargar sprites de botones
    cargarSpritesButtons();

    // Definir posiciones de botones
    definirRectangulos();

    // Reproducir música del menú
    SoundManager::getInstance().playMenuMusic();

    setMouseTracking(true);
    qDebug() << "[Menu] Menú principal inicializado";
}

Menu::~Menu()
{
}

void Menu::cargarRecursos()
{
    // Cargar recursos del menú desde ResourceManager
    ResourceManager::getInstance().cargarRecursosMenu();

    // Obtener sprite del menú principal
    fondoMenu = ResourceManager::getInstance().getSprite("menu_fondo");
    if (fondoMenu.isNull()) {
        qWarning() << "[Menu] No se pudo cargar la imagen de fondo del menú";
        fondoMenu = QPixmap(1024, 768);
        fondoMenu.fill(QColor(20, 20, 40));
    } else {
        qDebug() << "[Menu] Fondo cargado correctamente";
        fondoMenu = fondoMenu.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    
    // Cargar fondo de opciones
    fondoOpciones = ResourceManager::getInstance().getSprite("menu_opciones");
    if (!fondoOpciones.isNull()) {
        fondoOpciones = fondoOpciones.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        qDebug() << "[Menu] Fondo opciones cargado correctamente";
    }
}

void Menu::cargarSpritesButtons()
{
    auto &rm = ResourceManager::getInstance();
    
    // Botones del menú principal - usar sprites de jugar para inicio
    btnInicioNormal = rm.getSprite("btn_jugar_normal");
    btnInicioHover = rm.getSprite("btn_jugar_hover");
    btnOpcionesNormal = rm.getSprite("btn_opciones_normal");
    btnOpcionesHover = rm.getSprite("btn_opciones_hover");
    btnSalirNormal = rm.getSprite("btn_salir_normal");
    btnSalirHover = rm.getSprite("btn_salir_hover");
    
    // Botones del submenú opciones
    btnCreditosNormal = rm.getSprite("btn_creditos_normal");
    btnCreditosHover = rm.getSprite("btn_creditos_hover");
    
    // Sprites de sonido on/off
    spriteSoundOn = QPixmap(":/assets/fondos/sound_on.png");
    spriteSoundOff = QPixmap(":/assets/fondos/sound_off.png");
    
    if (!spriteSoundOn.isNull()) {
        spriteSoundOn = spriteSoundOn.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!spriteSoundOff.isNull()) {
        spriteSoundOff = spriteSoundOff.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "[Menu] Sprites de botones cargados";
}

void Menu::definirRectangulos()
{
    // Menú Principal - 3 botones alineados al centro (movidos 30px a la derecha)
    int btnWidth = 250;
    int btnHeight = 60;
    int startX = 340;
    int startY = 360;
    int spacing = 75;

    btnInicio = QRect(startX, startY, btnWidth, btnHeight);
    btnOpciones = QRect(startX, startY + spacing, btnWidth, btnHeight);
    btnSalir = QRect(startX, startY + spacing * 2, btnWidth, btnHeight);

    // Botón de sonido - esquina superior derecha
    btnSound = QRect(954, 10, 60, 60);

    // Submenú Opciones - Botones dentro del marco de menu_opciones
    // El marco tiene un área interior aproximadamente en el centro
    int opcionesWidth = 200;
    int opcionesHeight = 45;
    int opcionesCenterX = 512;  // Centro de la pantalla
    int opcionesStartY = 200;   // Debajo del título "OPCIONES"
    int opcionesSpacing = 60;

    btnNivel1 = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY, opcionesWidth, opcionesHeight);
    btnNivel2 = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY + opcionesSpacing, opcionesWidth, opcionesHeight);
    btnNivel3 = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY + opcionesSpacing * 2, opcionesWidth, opcionesHeight);
    btnCreditos = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY + opcionesSpacing * 3, opcionesWidth, opcionesHeight);
    btnDificultad = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY + opcionesSpacing * 4, opcionesWidth, opcionesHeight);
    btnVolver = QRect(opcionesCenterX - opcionesWidth/2, opcionesStartY + opcionesSpacing * 5, opcionesWidth, opcionesHeight);

    // Menú Dificultad
    int difWidth = 200;
    int difHeight = 50;
    int difCenterX = 512;
    int difStartY = 220;
    int difSpacing = 65;

    btnFacil = QRect(difCenterX - difWidth/2, difStartY, difWidth, difHeight);
    btnNormal = QRect(difCenterX - difWidth/2, difStartY + difSpacing, difWidth, difHeight);
    btnDificil = QRect(difCenterX - difWidth/2, difStartY + difSpacing * 2, difWidth, difHeight);
    btnVolverDificultad = QRect(difCenterX - difWidth/2, difStartY + difSpacing * 3, difWidth, difHeight);
}

void Menu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dibujar menú según estado actual
    switch (estadoActual) {
        case MENU_PRINCIPAL:
            dibujarMenuPrincipal(painter);
            break;
        case MENU_OPCIONES:
            dibujarMenuOpciones(painter);
            break;
        case MENU_CREDITOS:
            dibujarMenuCreditos(painter);
            break;
        case MENU_DIFICULTAD:
            dibujarMenuDificultad(painter);
            break;
    }
}

void Menu::dibujarBoton(QPainter &painter, const QRect &rect, const QString &texto,
                        const QPixmap &normal, const QPixmap &hover, bool isHover,
                        const QColor &colorBase)
{
    // Si hay sprite, usarlo; sino, dibujar fallback
    if (!normal.isNull() && !hover.isNull()) {
        QPixmap sprite = isHover ? hover : normal;
        painter.drawPixmap(rect, sprite.scaled(rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        // Fallback: dibujar botón con gradiente
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        if (isHover) {
            gradient.setColorAt(0, colorBase.lighter(140));
            gradient.setColorAt(1, colorBase);
        } else {
            gradient.setColorAt(0, colorBase);
            gradient.setColorAt(1, colorBase.darker(140));
        }
        
        painter.setBrush(gradient);
        painter.setPen(QPen(isHover ? colorBase.lighter(180) : colorBase.lighter(120), 2));
        painter.drawRoundedRect(rect, 8, 8);
    }
    
    // Texto del botón
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(rect, Qt::AlignCenter, texto);
}

void Menu::dibujarMenuPrincipal(QPainter &painter)
{
    // Dibujar fondo SIN opacidad
    painter.drawPixmap(0, 0, fondoMenu);

    // Solo subtítulo (debajo del título ya presente en el fondo)
    painter.setFont(QFont("Georgia", 14));
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(QRect(260, 280, 400, 30), Qt::AlignCenter, "Un viaje a través de la historia del conocimiento");

    // Botón INICIO - usando sprite si existe (sin texto extra)
    if (!btnInicioNormal.isNull()) {
        QPixmap sprite = hoverInicio ? btnInicioHover : btnInicioNormal;
        painter.drawPixmap(btnInicio, sprite.scaled(btnInicio.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        dibujarBoton(painter, btnInicio, "INICIO", btnInicioNormal, btnInicioHover, 
                     hoverInicio, QColor(100, 70, 40));
    }
    
    // Botón OPCIONES (sin texto extra si tiene sprite)
    if (!btnOpcionesNormal.isNull()) {
        QPixmap sprite = hoverOpciones ? btnOpcionesHover : btnOpcionesNormal;
        painter.drawPixmap(btnOpciones, sprite.scaled(btnOpciones.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        dibujarBoton(painter, btnOpciones, "OPCIONES", btnOpcionesNormal, btnOpcionesHover,
                     hoverOpciones, QColor(100, 70, 40));
    }
    
    // Botón SALIR (sin texto extra si tiene sprite)
    if (!btnSalirNormal.isNull()) {
        QPixmap sprite = hoverSalir ? btnSalirHover : btnSalirNormal;
        painter.drawPixmap(btnSalir, sprite.scaled(btnSalir.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        dibujarBoton(painter, btnSalir, "SALIR", btnSalirNormal, btnSalirHover,
                     hoverSalir, QColor(100, 70, 40));
    }

    // Botón de sonido
    dibujarBotonSonido(painter);

    // Pie de página
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(150, 150, 150));
    painter.drawText(QRect(0, 720, 1024, 30), Qt::AlignCenter, 
                     "WASD/Flechas: Mover | R: Recoger/Soltar | E: Entregar | ESC: Pausa");
}

void Menu::dibujarMenuOpciones(QPainter &painter)
{
    // Usar el fondo de opciones (menu_opciones.jpg)
    if (!fondoOpciones.isNull()) {
        painter.drawPixmap(0, 0, fondoOpciones);
    } else {
        // Fallback si no cargó
        painter.fillRect(0, 0, 1024, 768, QColor(40, 30, 20));
    }

    // Los botones van dentro del marco, debajo del título "OPCIONES" del fondo
    dibujarBoton(painter, btnNivel1, "NIVEL 1 - Gutenberg", QPixmap(), QPixmap(),
                 hoverNivel1, QColor(139, 90, 43));
    
    dibujarBoton(painter, btnNivel2, "NIVEL 2 - Alejandría", QPixmap(), QPixmap(),
                 hoverNivel2, QColor(180, 140, 70));
    
    dibujarBoton(painter, btnNivel3, "NIVEL 3 - Digital", QPixmap(), QPixmap(),
                 hoverNivel3, QColor(0, 100, 150));

    // CRÉDITOS - Ahora usa el mismo formato que los otros botones (sin sprites)
    dibujarBoton(painter, btnCreditos, "CRÉDITOS", QPixmap(), QPixmap(),
                 hoverCreditos, QColor(200, 50, 150));
    
    dibujarBoton(painter, btnDificultad, "DIFICULTAD", QPixmap(), QPixmap(),
                 hoverDificultad, QColor(80, 100, 80));
    
    dibujarBoton(painter, btnVolver, "VOLVER", QPixmap(), QPixmap(),
                 hoverVolver, QColor(120, 80, 60));

    // Mostrar dificultad actual debajo de los botones
    painter.setFont(QFont("Arial", 11));
    painter.setPen(QColor(200, 180, 140));
    QString difActual = "Dificultad: " + DifficultyManager::getInstance().getNombreDificultad();
    painter.drawText(QRect(0, 580, 1024, 25), Qt::AlignCenter, difActual);
}

void Menu::dibujarMenuCreditos(QPainter &painter)
{
    // Usar fondo de opciones
    if (!fondoOpciones.isNull()) {
        painter.drawPixmap(0, 0, fondoOpciones);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(40, 30, 20));
    }
    
    // Contenido de créditos dentro del marco
    painter.setFont(QFont("Arial", 13));
    painter.setPen(QColor(220, 200, 160));
    
    QStringList creditos = {
        "Desarrollado para el curso de Informatica II",
        "Programación Orientada a Objetos",
        "",
        "Universidad de Antioquia",
        "",
        "Equipo de Desarrollo:",
        "• Mauricio Aguas",
        "• Jharlin Castro",
        "",
        "2025"
    };
    
    int y = 200;
    for (const QString &linea : creditos) {
        painter.drawText(QRect(0, y, 1024, 25), Qt::AlignCenter, linea);
        y += 28;
    }
    
    // Botón Volver
    QRect btnVolverCreditos(412, 540, 200, 45);
    dibujarBoton(painter, btnVolverCreditos, "VOLVER", QPixmap(), QPixmap(),
                 hoverVolver, QColor(120, 80, 60));
    btnVolver = btnVolverCreditos;
}

void Menu::dibujarMenuDificultad(QPainter &painter)
{
    // Usar fondo de opciones
    if (!fondoOpciones.isNull()) {
        painter.drawPixmap(0, 0, fondoOpciones);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(40, 30, 20));
    }

    // Obtener dificultad actual
    int difActual = DifficultyManager::getInstance().getDificultad();

    // Botones de dificultad
    QColor colorFacil = (difActual == 0) ? QColor(50, 150, 50) : QColor(60, 100, 60);
    dibujarBoton(painter, btnFacil, "FÁCIL", QPixmap(), QPixmap(), hoverFacil, colorFacil);
    if (difActual == 0) {
        painter.setPen(QColor(100, 255, 100));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(btnFacil.adjusted(btnFacil.width() - 30, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, "✓");
    }
    
    QColor colorNormal = (difActual == 1) ? QColor(150, 150, 50) : QColor(100, 100, 60);
    dibujarBoton(painter, btnNormal, "NORMAL", QPixmap(), QPixmap(), hoverNormal, colorNormal);
    if (difActual == 1) {
        painter.setPen(QColor(255, 255, 100));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(btnNormal.adjusted(btnNormal.width() - 30, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, "✓");
    }
    
    QColor colorDificil = (difActual == 2) ? QColor(150, 50, 50) : QColor(100, 60, 60);
    dibujarBoton(painter, btnDificil, "DIFÍCIL", QPixmap(), QPixmap(), hoverDificil, colorDificil);
    if (difActual == 2) {
        painter.setPen(QColor(255, 100, 100));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(btnDificil.adjusted(btnDificil.width() - 30, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, "✓");
    }
    
    // Botón VOLVER
    dibujarBoton(painter, btnVolverDificultad, "VOLVER", QPixmap(), QPixmap(),
                 hoverVolverDificultad, QColor(120, 80, 60));

    // Descripción de la dificultad
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(180, 160, 120));
    QString descripcion;
    switch (difActual) {
        case 0: descripcion = "Más tiempo, menos enemigos"; break;
        case 1: descripcion = "Experiencia balanceada"; break;
        case 2: descripcion = "Menos tiempo, más enemigos"; break;
    }
    painter.drawText(QRect(0, 520, 1024, 25), Qt::AlignCenter, descripcion);
}

void Menu::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->position().x();
    int y = event->position().y();

    switch (estadoActual) {
        case MENU_PRINCIPAL:
            verificarHoverPrincipal(x, y);
            break;
        case MENU_OPCIONES:
            verificarHoverOpciones(x, y);
            break;
        case MENU_CREDITOS:
            verificarHoverCreditos(x, y);
            break;
        case MENU_DIFICULTAD:
            verificarHoverDificultad(x, y);
            break;
    }
    
    update();
}

void Menu::verificarHoverPrincipal(int x, int y)
{
    hoverInicio = btnInicio.contains(x, y);
    hoverOpciones = btnOpciones.contains(x, y);
    hoverSalir = btnSalir.contains(x, y);
    hoverSound = btnSound.contains(x, y);

    if (hoverInicio || hoverOpciones || hoverSalir || hoverSound) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void Menu::verificarHoverOpciones(int x, int y)
{
    hoverNivel1 = btnNivel1.contains(x, y);
    hoverNivel2 = btnNivel2.contains(x, y);
    hoverNivel3 = btnNivel3.contains(x, y);
    hoverCreditos = btnCreditos.contains(x, y);
    hoverDificultad = btnDificultad.contains(x, y);
    hoverVolver = btnVolver.contains(x, y);

    if (hoverNivel1 || hoverNivel2 || hoverNivel3 || hoverCreditos || hoverDificultad || hoverVolver) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void Menu::verificarHoverCreditos(int x, int y)
{
    hoverVolver = btnVolver.contains(x, y);
    setCursor(hoverVolver ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

void Menu::verificarHoverDificultad(int x, int y)
{
    hoverFacil = btnFacil.contains(x, y);
    hoverNormal = btnNormal.contains(x, y);
    hoverDificil = btnDificil.contains(x, y);
    hoverVolverDificultad = btnVolverDificultad.contains(x, y);

    if (hoverFacil || hoverNormal || hoverDificil || hoverVolverDificultad) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void Menu::mousePressEvent(QMouseEvent *event)
{
    int x = event->position().x();
    int y = event->position().y();

    // Verificar click en botón de sonido (siempre visible)
    if (btnSound.contains(x, y)) {
        onSoundToggleClicked();
        return;
    }

    switch (estadoActual) {
        case MENU_PRINCIPAL:
            if (btnInicio.contains(x, y)) onInicioClicked();
            else if (btnOpciones.contains(x, y)) onOpcionesClicked();
            else if (btnSalir.contains(x, y)) onSalirClicked();
            break;
            
        case MENU_OPCIONES:
            if (btnNivel1.contains(x, y)) onNivel1Clicked();
            else if (btnNivel2.contains(x, y)) onNivel2Clicked();
            else if (btnNivel3.contains(x, y)) onNivel3Clicked();
            else if (btnCreditos.contains(x, y)) onCreditosClicked();
            else if (btnDificultad.contains(x, y)) onDificultadClicked();
            else if (btnVolver.contains(x, y)) onVolverClicked();
            break;
            
        case MENU_CREDITOS:
            if (btnVolver.contains(x, y)) onVolverClicked();
            break;
            
        case MENU_DIFICULTAD:
            if (btnFacil.contains(x, y)) onFacilClicked();
            else if (btnNormal.contains(x, y)) onNormalClicked();
            else if (btnDificil.contains(x, y)) onDificilClicked();
            else if (btnVolverDificultad.contains(x, y)) onVolverClicked();
            break;
    }
}

// Slots del menú principal
void Menu::onInicioClicked()
{
    qDebug() << "[Menu] Inicio - Comenzar desde Nivel 1";
    emit iniciarNivel(1);
    close();
}

void Menu::onOpcionesClicked()
{
    qDebug() << "[Menu] Abriendo submenú de opciones";
    estadoActual = MENU_OPCIONES;
    definirRectangulos();
    update();
}

void Menu::onSalirClicked()
{
    qDebug() << "[Menu] Salir del juego";
    QApplication::quit();
}

// Slots del submenú opciones
void Menu::onCreditosClicked()
{
    qDebug() << "[Menu] Mostrando créditos";
    estadoActual = MENU_CREDITOS;
    update();
}

void Menu::onDificultadClicked()
{
    qDebug() << "[Menu] Abriendo menú de dificultad";
    estadoActual = MENU_DIFICULTAD;
    update();
}

void Menu::onNivel1Clicked()
{
    qDebug() << "[Menu] Iniciar Nivel 1 - Taller de Gutenberg";
    emit iniciarNivel(1);
    close();
}

void Menu::onNivel2Clicked()
{
    qDebug() << "[Menu] Iniciar Nivel 2 - Biblioteca de Alejandría";
    emit iniciarNivel(2);
    close();
}

void Menu::onNivel3Clicked()
{
    qDebug() << "[Menu] Iniciar Nivel 3 - Revolución Digital";
    emit iniciarNivel(3);
    close();
}

void Menu::onVolverClicked()
{
    qDebug() << "[Menu] Volviendo al menú anterior";
    if (estadoActual == MENU_CREDITOS || estadoActual == MENU_DIFICULTAD) {
        estadoActual = MENU_OPCIONES;
        definirRectangulos();
    } else if (estadoActual == MENU_OPCIONES) {
        estadoActual = MENU_PRINCIPAL;
    }
    update();
}

// Slots de dificultad
void Menu::onFacilClicked()
{
    qDebug() << "[Menu] Dificultad cambiada a: FÁCIL";
    DifficultyManager::getInstance().setDificultad(0);
    emit cambiarDificultad(0);
    update();
}

void Menu::onNormalClicked()
{
    qDebug() << "[Menu] Dificultad cambiada a: NORMAL";
    DifficultyManager::getInstance().setDificultad(1);
    emit cambiarDificultad(1);
    update();
}

void Menu::onDificilClicked()
{
    qDebug() << "[Menu] Dificultad cambiada a: DIFÍCIL";
    DifficultyManager::getInstance().setDificultad(2);
    emit cambiarDificultad(2);
    update();
}

void Menu::onSoundToggleClicked()
{
    SoundManager::getInstance().toggleSound();
    qDebug() << "[Menu] Sonido:" << (SoundManager::getInstance().isSoundEnabled() ? "ON" : "OFF");
    update();
}

void Menu::dibujarBotonSonido(QPainter &painter)
{
    bool soundOn = SoundManager::getInstance().isSoundEnabled();
    QPixmap sprite = soundOn ? spriteSoundOn : spriteSoundOff;
    
    if (!sprite.isNull()) {
        // Efecto hover
        if (hoverSound) {
            painter.setOpacity(1.0);
        } else {
            painter.setOpacity(0.8);
        }
        painter.drawPixmap(btnSound, sprite.scaled(btnSound.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        painter.setOpacity(1.0);
    } else {
        // Fallback si no hay sprite
        QColor color = soundOn ? QColor(50, 150, 50) : QColor(150, 50, 50);
        if (hoverSound) color = color.lighter(130);
        
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRoundedRect(btnSound, 8, 8);
        
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(Qt::white);
        painter.drawText(btnSound, Qt::AlignCenter, soundOn ? "ON" : "OFF");
    }
}
