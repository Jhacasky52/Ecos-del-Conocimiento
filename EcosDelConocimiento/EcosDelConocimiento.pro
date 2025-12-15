QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Nombre del ejecutable
TARGET = EcosDelConocimiento
TEMPLATE = app

# =============================================================================
# INCLUDEPATH - Permite usar includes simples como #include "Jugador.h"
# =============================================================================
INCLUDEPATH += \
    src/core \
    src/entities \
    src/gui \
    src/managers \
    src/physics

# =============================================================================
# HEADERS - Archivos de cabecera (.h)
# =============================================================================
HEADERS += \
    src/core/GameManager.h \
    src/entities/Entidad.h \
    src/entities/Jugador.h \
    src/entities/Automata.h \
    src/entities/Letra.h \
    src/entities/Cuervo.h \
    src/entities/Pergamino.h \
    src/entities/Cubeta.h \
    src/entities/FuegoAire.h \
    src/entities/FuegoPiso.h \
    src/entities/Virus.h \
    src/entities/Firewall.h \
    src/entities/Nodo.h \
    src/entities/Fragmento.h \
    src/entities/CampoFuerza.h \
    src/entities/Guardian.h \
    src/gui/MainWindow.h \
    src/gui/Menu.h \
    src/gui/Nivel1.h \
    src/gui/Nivel2.h \
    src/gui/Nivel3.h \
    src/managers/ResourceManager.h \
    src/managers/DifficultyManager.h \
    src/managers/SoundManager.h \
    src/physics/PhysicsEngine.h

# =============================================================================
# SOURCES - Archivos de implementación (.cpp)
# =============================================================================
SOURCES += \
    src/main.cpp \
    src/core/GameManager.cpp \
    src/entities/Entidad.cpp \
    src/entities/Jugador.cpp \
    src/entities/Automata.cpp \
    src/entities/Letra.cpp \
    src/entities/Cuervo.cpp \
    src/entities/Pergamino.cpp \
    src/entities/Cubeta.cpp \
    src/entities/FuegoAire.cpp \
    src/entities/FuegoPiso.cpp \
    src/entities/Virus.cpp \
    src/entities/Firewall.cpp \
    src/entities/Nodo.cpp \
    src/entities/Fragmento.cpp \
    src/entities/CampoFuerza.cpp \
    src/entities/Guardian.cpp \
    src/gui/MainWindow.cpp \
    src/gui/Menu.cpp \
    src/gui/Nivel1.cpp \
    src/gui/Nivel2.cpp \
    src/gui/Nivel3.cpp \
    src/managers/ResourceManager.cpp \
    src/managers/DifficultyManager.cpp \
    src/physics/PhysicsEngine.cpp

# =============================================================================
# RESOURCES - Archivo de recursos Qt
# =============================================================================
RESOURCES += \
    resources.qrc

# =============================================================================
# CONFIGURACIÓN ADICIONAL
# =============================================================================

# Warnings
QMAKE_CXXFLAGS += -Wall -Wextra

# Directorio de salida
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
