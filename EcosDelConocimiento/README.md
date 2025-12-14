# 🎮 Ecos del Conocimiento

## 📁 Estructura del Proyecto Organizada

```
EcosDelConocimiento/
│
├── EcosDelConocimiento.pro    # Archivo de proyecto Qt
├── resources.qrc              # Recursos Qt (imágenes, etc.)
│
├── src/
│   ├── main.cpp               # Punto de entrada
│   │
│   ├── core/                  # Núcleo del juego
│   │   ├── GameManager.h
│   │   └── GameManager.cpp
│   │
│   ├── entities/              # Entidades del juego
│   │   ├── Entidad.h          # 🆕 CLASE BASE (herencia propia)
│   │   ├── Entidad.cpp
│   │   ├── Jugador.h          # Hereda de Entidad
│   │   ├── Jugador.cpp
│   │   ├── Automata.h         # Hereda de Entidad
│   │   ├── Automata.cpp
│   │   ├── Letra.h            # Hereda de Entidad
│   │   └── Letra.cpp
│   │
│   ├── gui/                   # Interfaz gráfica
│   │   ├── MainWindow.h
│   │   ├── MainWindow.cpp
│   │   ├── Menu.h
│   │   ├── Menu.cpp
│   │   ├── Nivel1.h
│   │   └── Nivel1.cpp
│   │
│   ├── managers/              # Gestores
│   │   ├── ResourceManager.h  # Singleton + Excepciones
│   │   ├── ResourceManager.cpp
│   │   ├── DifficultyManager.h # 🆕 Sistema de dificultad
│   │   └── DifficultyManager.cpp
│   │
│   └── physics/               # 🆕 Motor de física
│       ├── PhysicsEngine.h    # 3 modelos físicos documentados
│       └── PhysicsEngine.cpp
│
├── assets/
│   ├── fondos/
│   │   ├── menu.png
│   │   └── nivel1_taller_gutenberg.png
│   └── sprites/
│       ├── nivel1_automata_jugador_frente.png
│       ├── nivel1_automata_jugador_espalda.png
│       ├── nivel1_automata_jugador_izquierda.png
│       ├── nivel1_automata_jugador_derecha.png
│       ├── nivel1_mesa_arriba.png
│       ├── nivel1_mesa_abajo.png
│       ├── nivel1_letras.png
│       ├── nivel1_prensa_imprenta.png
│       └── nivel1_prensa_imprenta2.png
│
└── docs/
    └── README.md
```

---

## ✅ Requisitos Cumplidos

### 1. Físicas (3 modelos) ✅
| Modelo | Archivo | Descripción |
|--------|---------|-------------|
| Caída libre | `Letra.cpp`, `PhysicsEngine.h` | `velocidadY += GRAVEDAD * deltaTime` |
| Rebote con fricción | `Letra.cpp`, `PhysicsEngine.h` | `velocidadY = -velocidadY * FRICCION` |
| Movimiento parabólico | `Letra.cpp`, `PhysicsEngine.h` | `lanzar(angulo, velocidad)` con componentes Vx, Vy |

### 2. POO y Memoria Dinámica ✅
- Clases: `Entidad`, `Jugador`, `Automata`, `Letra`, `Nivel1`, `Menu`, etc.
- Uso de `new`/`delete` en `Nivel1` para crear entidades
- Destructores que liberan memoria

### 3. Herencia Propia (NO de Qt) ✅
```
Entidad (clase base abstracta)
    ├── Jugador
    ├── Automata
    └── Letra
```

### 4. Contenedores Qt ✅
- `QVector<Letra*>` - Almacena letras del nivel
- `QMap<QString, QPixmap>` - Caché de sprites
- `QSet<int>` - Teclas presionadas
- `QVector<QRect>` - Colisiones

### 5. Excepciones ✅
```cpp
// En ResourceManager.h
try {
    QPixmap img(ruta);
    if (img.isNull()) {
        throw std::runtime_error("No se pudo cargar...");
    }
} catch (const std::exception& e) {
    // Usar imagen fallback
}

// En Nivel1.cpp
try {
    Letra *letra = new Letra(this);
    if (!letra) throw std::bad_alloc();
} catch (const std::bad_alloc& e) {
    // Manejar error de memoria
}
```

### 6. Sistema de Dificultad ✅
```cpp
// DifficultyManager.h
enum class Dificultad { FACIL, NORMAL, DIFICIL };

// Parámetros que cambian según dificultad:
- Tiempo límite: 90s / 60s / 40s
- Velocidad jugador: 350 / 300 / 250 px/s
- Número de letras: 15 / 25 / 35
- Puntos por letra: 50 / 100 / 200
- Progresión automática cada 3 rondas
```

### 7. GUI Qt ✅
- `QWidget` como base
- `QPainter` para renderizado
- Eventos de teclado y mouse
- Timer para game loop (60 FPS)

---

## 🔧 Cómo Usar

### Compilar
```bash
cd EcosDelConocimiento
qmake EcosDelConocimiento.pro
make
./build/EcosDelConocimiento
```

### Controles
- **WASD / Flechas**: Mover jugador
- **R**: Recoger/Soltar letra
- **E**: Entregar letra / Interactuar con autómata
- **Click**: Recoger letra cercana
- **ESC**: Salir

---

## 📝 Notas sobre los Includes

Gracias al `INCLUDEPATH` en el archivo `.pro`, los includes pueden seguir siendo simples:

```cpp
// Esto funciona gracias a INCLUDEPATH en .pro
#include "Jugador.h"      // En lugar de "entities/Jugador.h"
#include "Nivel1.h"       // En lugar de "gui/Nivel1.h"
#include "ResourceManager.h"  // En lugar de "managers/ResourceManager.h"
```

---

## 📊 Diagrama de Clases (Pendiente)

Necesitas crear un diagrama UML que muestre:

**Capa Lógica:**
- `Entidad` (abstracta) → `Jugador`, `Automata`, `Letra`
- `ResourceManager` (Singleton)
- `DifficultyManager` (Singleton)
- `PhysicsEngine` (utilidades estáticas)

**Capa GUI:**
- `MainWindow` → `Nivel1`
- `Menu`

---

## 🚀 Próximos Pasos

1. [ ] Copiar tus imágenes a la carpeta `assets/`
2. [ ] Generar diagrama de clases UML
3. [ ] Configurar repositorio Git con ramas
4. [ ] Probar compilación
5. [ ] Ajustar rutas en `resources.qrc` si es necesario
