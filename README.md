# 🎮 Ecos del Conocimiento



<p align="center">
  <strong>Un viaje interactivo a través de la historia del conocimiento humano</strong>
</p>

<p align="center">
  <a href="#-descripción">Descripción</a> •
  <a href="#-características">Características</a> •
  <a href="#-niveles">Niveles</a> •
  <a href="#-instalación">Instalación</a> •
  <a href="#-controles">Controles</a> •
  <a href="#-arquitectura">Arquitectura</a>
</p>

---

## 📖 Descripción

**Ecos del Conocimiento** es un videojuego educativo desarrollado en C++ con Qt que lleva al jugador a través de tres épocas fundamentales en la historia de la preservación y difusión del conocimiento:

1. 🖨️ **La Imprenta de Gutenberg** (Siglo XV)
2. 🔥 **La Biblioteca de Alejandría** (Antigüedad)
3. 💻 **La Revolución Digital** (Era Moderna)

Cada nivel presenta mecánicas únicas y desafíos que reflejan los retos de cada época para preservar el conocimiento.

---

## ✨ Características

### 🎯 Jugabilidad
- **3 niveles únicos** con mecánicas diferenciadas
- **Sistema de dificultad dinámico** (Fácil, Normal, Difícil)
- **Autómata con IA** que piensa, decide, actúa y memoriza
- **Sistema de puntuación** acumulativo entre niveles

### 🔧 Técnicas
- **Múltiples modelos físicos:**
  - Movimiento parabólico (lanzamiento de cubeta)
  - Movimiento oscilatorio/sinusoidal (virus y fragmentos)
  - Campos de fuerza con atracción/repulsión
  - Física de salto con gravedad
  - Rebote con coeficiente de restitución

- **Programación Orientada a Objetos:**
  - Herencia propia (no de Qt) con clase base `Entidad`
  - 14 clases que heredan de `Entidad`
  - Polimorfismo con métodos virtuales puros
  - Encapsulamiento completo

- **Patrones de diseño:**
  - Singleton (DifficultyManager, ResourceManager, SoundManager)
  - State Pattern (máquinas de estados en múltiples entidades)
  - Factory Pattern (creación de entidades)

### 🎨 Multimedia
- **Sistema de sonido completo** con música por nivel
- **Efectos de sonido** para acciones del juego
- **Toggle de sonido** accesible en todo momento
- **Sprites y animaciones** personalizados

---

## 🎮 Niveles

### Nivel 1: Taller de Gutenberg 🖨️
> *Alemania, 1450*

Ayuda a Johannes Gutenberg a imprimir la primera Biblia. Recoge las letras correctas y llévalas a la imprenta en el orden correcto mientras un autómata inteligente intenta competir contigo.

**Mecánicas:**
- Recolección de letras
- Entrega en orden específico
- Competencia con autómata IA
- Límite de tiempo

**Autómata IA:**
- 🧠 **Pensar:** Evalúa distancias a todas las letras
- 🎯 **Decidir:** Selecciona la mejor letra objetivo
- 🏃 **Actuar:** Se mueve y recoge letras
- 💾 **Memorizar:** Recuerda letras ya procesadas

---

### Nivel 2: Biblioteca de Alejandría 🔥
> *Egipto, 48 a.C.*

La Gran Biblioteca está en llamas. Rescata los pergaminos antiguos antes de que se quemen y protégelos del cuervo ladrón. Usa la cubeta de agua para apagar el fuego.

**Mecánicas:**
- Scrolling horizontal (mundo de 2500px)
- Física de salto con gravedad
- Lanzamiento parabólico de cubeta de agua
- Sistema de fuego con propagación
- Estados de pergaminos (Normal → Poco Fuego → Mucho Fuego → Quemado)
- Enemigo cuervo con IA de robo

**Física implementada:**
```
Salto: velocidadY += GRAVEDAD * deltaTime
Cubeta: trayectoria parabólica con ángulo de 45°
```

---

### Nivel 3: Revolución Digital 💻
> *Era Moderna*

Navega por el ciberespacio como un paquete de datos. Esquiva virus, atraviesa firewalls y recolecta fragmentos de información para llevarlos a la terminal central.

**Mecánicas:**
- Movimiento libre en 2D
- Virus con movimiento oscilatorio
- Firewalls estáticos y móviles
- Nodos de regeneración de energía
- Campos de fuerza (atracción/repulsión)
- Guardián perseguidor

**Física implementada:**
```cpp
// Movimiento oscilatorio de virus
fase += frecuencia * deltaTime;
oscX = sin(fase) * amplitud;
oscY = cos(fase * 0.7f) * amplitud * 0.5f;

// Campos de fuerza
fuerza = intensidad * (1 - distancia/radio);
```

---

## 🛠️ Instalación

### Requisitos
- Qt 6.x o superior
- Compilador C++17 compatible
- Qt Multimedia module

### Compilación

```bash
# Clonar el repositorio
git clone https://github.com/tu-usuario/EcosDelConocimiento.git
cd EcosDelConocimiento

# Compilar con qmake
qmake EcosDelConocimiento.pro
make

# O usar Qt Creator
# Abrir EcosDelConocimiento.pro y compilar
```

### Estructura del Proyecto

```
EcosDelConocimiento/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── GameManager.h/.cpp
│   ├── entities/
│   │   ├── Entidad.h/.cpp          # Clase base abstracta
│   │   ├── Jugador.h/.cpp
│   │   ├── Automata.h/.cpp         # IA del nivel 1
│   │   ├── Letra.h/.cpp
│   │   ├── Cuervo.h/.cpp           # Enemigo nivel 2
│   │   ├── Pergamino.h/.cpp
│   │   ├── Cubeta.h/.cpp           # Física parabólica
│   │   ├── FuegoAire.h/.cpp
│   │   ├── FuegoPiso.h/.cpp
│   │   ├── Virus.h/.cpp            # Movimiento oscilatorio
│   │   ├── Firewall.h/.cpp
│   │   ├── Nodo.h/.cpp
│   │   ├── Fragmento.h/.cpp
│   │   ├── CampoFuerza.h/.cpp      # Física de campos
│   │   └── Guardian.h/.cpp
│   ├── gui/
│   │   ├── MainWindow.h/.cpp
│   │   ├── Menu.h/.cpp
│   │   ├── Nivel1.h/.cpp
│   │   ├── Nivel2.h/.cpp
│   │   └── Nivel3.h/.cpp
│   ├── managers/
│   │   ├── DifficultyManager.h     # Singleton - Dificultad
│   │   ├── ResourceManager.h/.cpp  # Singleton - Recursos
│   │   └── SoundManager.h          # Singleton - Audio
│   └── physics/
│       └── PhysicsEngine.h/.cpp
├── assets/
│   ├── fondos/
│   ├── sprites/
│   └── sounds/
├── resources.qrc
├── EcosDelConocimiento.pro
└── README.md
```

---

## 🎮 Controles

### Menú Principal
| Acción | Control |
|--------|---------|
| Navegar | Mouse |
| Seleccionar | Click Izquierdo |
| Sonido On/Off | Botón esquina superior derecha |

### Nivel 1 - Taller de Gutenberg
| Acción | Control |
|--------|---------|
| Mover | W, A, S, D / Flechas |
| Recoger/Soltar letra | Click en letra / Acercarse a imprenta |

### Nivel 2 - Biblioteca de Alejandría
| Acción | Control |
|--------|---------|
| Mover | A, D / ← → |
| Saltar | W / ↑ |
| Lanzar agua | Automático al tener cubeta |
| Recoger pergamino | Acercarse |

### Nivel 3 - Revolución Digital
| Acción | Control |
|--------|---------|
| Mover | W, A, S, D / Flechas |
| Recoger fragmento | Click / Acercarse |

### General
| Acción | Control |
|--------|---------|
| Pausar | ESC |
| Sonido On/Off | Botón en pantalla |

---

## 🏗️ Arquitectura

### Diagrama de Clases (Simplificado)

```
                         ┌─────────────────┐
                         │     Entidad     │
                         │   (Abstracta)   │
                         └────────┬────────┘
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        │                         │                         │
        ▼                         ▼                         ▼
   ┌─────────┐              ┌──────────┐              ┌─────────┐
   │ Jugador │              │ Automata │              │  Letra  │
   └─────────┘              └──────────┘              └─────────┘
        │
        ├── Cuervo, Pergamino, Cubeta, FuegoAire, FuegoPiso
        │
        └── Virus, Firewall, Nodo, Fragmento, CampoFuerza, Guardian


   ┌───────────────────┐  ┌───────────────────┐  ┌───────────────────┐
   │ DifficultyManager │  │  ResourceManager  │  │   SoundManager    │
   │    «Singleton»    │  │    «Singleton»    │  │    «Singleton»    │
   └───────────────────┘  └───────────────────┘  └───────────────────┘
```

### Capas de la Aplicación

```
┌─────────────────────────────────────────────────────────┐
│                      CAPA GUI                           │
│  MainWindow, Menu, Nivel1, Nivel2, Nivel3               │
├─────────────────────────────────────────────────────────┤
│                   CAPA LÓGICA                           │
│  GameManager, DifficultyManager, ResourceManager,       │
│  SoundManager, PhysicsEngine                            │
├─────────────────────────────────────────────────────────┤
│                  CAPA ENTIDADES                         │
│  Entidad (base), Jugador, Automata, Letra, Cuervo,     │
│  Pergamino, Cubeta, Fuego*, Virus, Firewall, Nodo,     │
│  Fragmento, CampoFuerza, Guardian                       │
└─────────────────────────────────────────────────────────┘
```

---

## 📊 Sistema de Dificultad

| Parámetro | Fácil | Normal | Difícil |
|-----------|-------|--------|---------|
| **Nivel 1** |
| Tiempo límite | 120s | 90s | 60s |
| Puntos por letra | 50 | 100 | 200 |
| Penalización error | 5 | 10 | 25 |
| **Nivel 2** |
| Tiempo límite | 240s | 180s | 120s |
| Intervalo spawn fuego | 2.5s | 1.5s | 0.8s |
| Pergaminos para ganar | 4 | 6 | 8 |
| Velocidad cuervo | 120 | 180 | 250 |
| **Nivel 3** |
| Cantidad de virus | 3 | 5 | 8 |
| Velocidad guardian | 150 | 180 | 220 |
| Fragmentos necesarios | 3 | 5 | 7 |

---

## 🔊 Sistema de Audio

| Sonido | Archivo | Uso |
|--------|---------|-----|
| Menú Principal | `menu_principal.wav` | Música del menú |
| Nivel 1 | `nivel1_background.mp3` | Música ambiente |
| Prensa | `nivel1_prensa.wav` | Efecto al imprimir |
| Nivel 2 | `nivel2_background.wav` | Música ambiente |
| Nivel 3 | `nivel3_background.wav` | Música ambiente |
| Victoria | `victoria.mp3` | Al ganar nivel |
| Derrota | `derrota.wav` | Al perder nivel |

---

## 🧪 Modelos Físicos Implementados

### 1. Movimiento Parabólico
```cpp
// Cubeta.cpp
velocidadY += GRAVEDAD * deltaTime;
posX += velocidadX * deltaTime;
posY += velocidadY * deltaTime;
```

### 2. Movimiento Oscilatorio
```cpp
// Virus.cpp
fase += frecuencia * deltaTime;
float oscX = sin(fase) * amplitud;
float oscY = cos(fase * 0.7f) * amplitud * 0.5f;
```

### 3. Campos de Fuerza
```cpp
// CampoFuerza.cpp
float factor = intensidad * (1.0f - dist / radio);
fx = nx * factor;  // Fuerza en X
fy = ny * factor;  // Fuerza en Y
```

### 4. Física de Salto
```cpp
// Nivel2.cpp
if (!enSuelo) {
    velocidadY += GRAVEDAD * deltaTime;
}
if (saltar && enSuelo) {
    velocidadY = FUERZA_SALTO;
}
```

### 5. Rebote con Restitución
```cpp
// Firewall.cpp
velocidadY *= -COEFICIENTE_RESTITUCION;
```

---

## 🤖 Autómata con IA

El autómata del Nivel 1 implementa un ciclo completo de IA:

```cpp
void Automata::actualizar(float deltaTime) {
    pensar();      // Evalúa el entorno
    decidir();     // Toma decisiones
    actuar(deltaTime);  // Ejecuta acciones
    memorizar();   // Guarda información
}
```

### Estados del Autómata
```
ESPERANDO → BUSCANDO → RECOGIENDO → LLEVANDO → ENTREGANDO
    ↑                                              │
    └──────────────────────────────────────────────┘
```

---

## 📝 Requisitos del Proyecto Cumplidos

| Requisito | Estado | Implementación |
|-----------|--------|----------------|
| 3+ Modelos Físicos | ✅ | Parabólico, Oscilatorio, Campos, Gravedad, Rebote |
| POO | ✅ | Clases, Herencia, Polimorfismo, Encapsulamiento |
| Memoria Dinámica | ✅ | new/delete en niveles y entidades |
| Herencia Propia | ✅ | Entidad → 14 clases hijas |
| Sistema de Dificultad | ✅ | DifficultyManager con 3 niveles |
| Contenedores | ✅ | QVector, QMap, QSet |
| GUI Qt | ✅ | QWidget, QPainter, QTimer, QMediaPlayer |
| Autómata IA | ✅ | Pensar, Decidir, Actuar, Memorizar |
| Repositorio Git | ✅ | Commits regulares con ramas |

---

## 🖼️ Capturas de Pantalla

### Menú Principal
*Pantalla de inicio con opciones de juego y configuración*

### Nivel 1 - Taller de Gutenberg
*Recolecta letras y compite con el autómata*

### Nivel 2 - Biblioteca de Alejandría
*Rescata pergaminos del fuego*

### Nivel 3 - Revolución Digital
*Navega el ciberespacio y evita los virus*

---

## 📄 Licencia

Este proyecto fue desarrollado como proyecto final para la asignatura de Informática II.

Universidad EAFIT - 2024

---

## 🙏 Agradecimientos

- Universidad EAFIT
- Qt Framework
- Inspiración histórica: Johannes Gutenberg, Biblioteca de Alejandría, Era Digital

---

<p align="center">
  <i>Preservando el conocimiento, una era a la vez.</i>
</p>

<p align="center">
  Desarrollado con ❤️ usando C++ y Qt
</p>
