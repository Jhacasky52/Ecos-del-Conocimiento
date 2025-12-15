#include "Nivel3.h"
#include "ResourceManager.h"
#include "DifficultyManager.h"
#include <QRandomGenerator>
#include <cmath>

Nivel3::Nivel3(QObject *parent)
    : QObject(parent),
      jugador(nullptr),
      velocidadJugadorX(0.0f),
      velocidadJugadorY(0.0f),
      fragmentoRecogidoIndex(-1),
      fragmentosEntregados(0),
      terminalX(512.0f),
      terminalY(100.0f),
      terminalAncho(120.0f),
      terminalAlto(150.0f),
      pulsoTerminal(0.0f),
      guardianX(100.0f),
      guardianY(400.0f),
      guardianVelX(0.0f),
      guardianVelY(0.0f),
      guardianActivo(true),
      guardianPulso(0.0f),
      energia(100.0f),
      energiaMaxima(100.0f),
      escudoActivo(false),
      tiempoEscudo(0.0f),
      offsetGrid(0.0f),
      tiempoRestante(TIEMPO_LIMITE),
      juegoActivo(true),
      puntuacion(0),
      tiempoInvulnerable(0.0f)
{
    cargarRecursos();
    jugador = new Jugador(this);
    jugador->setPosicion(512.0f, 600.0f);
    jugador->setLimites(1024.0f, 768.0f);
    
    crearFragmentos();
    crearVirus();
    crearNodos();
    crearFirewalls();
    crearCamposFuerza();
    generarParticulas();
    iniciarJuego();
}

Nivel3::~Nivel3() { if (jugador) delete jugador; }

void Nivel3::cargarRecursos()
{
    ResourceManager::getInstance().cargarRecursosNivel3();
    fondoNivel = ResourceManager::getInstance().getSprite("fondo_nivel3");
    if (!fondoNivel.isNull()) fondoNivel = fondoNivel.scaled(1024, 768, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    spritePacket = ResourceManager::getInstance().getSprite("nivel3_packet");
    spritePacketEscudo = ResourceManager::getInstance().getSprite("nivel3_packet_escudo");
    spriteVirus = ResourceManager::getInstance().getSprite("nivel3_virus");
    spriteFragmento = ResourceManager::getInstance().getSprite("nivel3_fragmento");
    spriteNodo = ResourceManager::getInstance().getSprite("nivel3_nodo");
    spriteFirewall = ResourceManager::getInstance().getSprite("nivel3_firewall");
    spriteTerminal = ResourceManager::getInstance().getSprite("nivel3_terminal");
    spriteGuardian = ResourceManager::getInstance().getSprite("nivel3_guardian");
}

void Nivel3::iniciarJuego()
{
    tiempoRestante = DifficultyManager::getInstance().getTiempoLimite() * 1.5f;
    juegoActivo = true;
    puntuacion = 0;
    fragmentosEntregados = 0;
    energia = energiaMaxima;
}

void Nivel3::crearFragmentos()
{
    fragmentos.clear();
    for (int i = 0; i < NUM_FRAGMENTOS; i++) {
        Fragmento f;
        f.x = 100.0f + QRandomGenerator::global()->bounded(824);
        f.y = 250.0f + QRandomGenerator::global()->bounded(400);
        f.recogido = false;
        f.entregado = false;
        f.oscilacionFase = QRandomGenerator::global()->bounded(1000) / 100.0f;
        // Nuevos parámetros de movimiento tipo virus
        f.velocidadBaseX = (QRandomGenerator::global()->bounded(100) - 50) * 1.5f;  // -75 a 75
        f.velocidadBaseY = (QRandomGenerator::global()->bounded(100) - 50) * 1.5f;
        f.amplitud = 20.0f + QRandomGenerator::global()->bounded(30);  // 20-50
        f.frecuencia = 1.0f + QRandomGenerator::global()->bounded(100) / 100.0f;  // 1.0-2.0
        fragmentos.append(f);
    }
}

void Nivel3::crearVirus()
{
    virus.clear();
    int numVirus = NUM_VIRUS + (DifficultyManager::getInstance().getDificultad() == 2 ? 2 : 0);
    for (int i = 0; i < numVirus; i++) {
        Virus v;
        v.x = 100.0f + QRandomGenerator::global()->bounded(824);
        v.y = 200.0f + QRandomGenerator::global()->bounded(450);
        v.velocidadBaseX = (QRandomGenerator::global()->bounded(100) - 50) * 2.0f;
        v.velocidadBaseY = (QRandomGenerator::global()->bounded(100) - 50) * 2.0f;
        v.amplitud = 30.0f + QRandomGenerator::global()->bounded(40);
        v.frecuencia = 1.5f + QRandomGenerator::global()->bounded(100) / 100.0f;
        v.fase = QRandomGenerator::global()->bounded(1000) / 100.0f;
        v.radio = 35.0f;
        virus.append(v);
    }
}

void Nivel3::crearNodos()
{
    nodos.clear();
    float pos[][2] = {{150,200},{874,200},{150,600},{874,600}};
    for (int i = 0; i < NUM_NODOS; i++) {
        Nodo n;
        n.x = pos[i][0]; n.y = pos[i][1];
        n.radio = 50.0f; n.activo = true;
        n.tiempoRecarga = 0.0f;
        n.pulso = QRandomGenerator::global()->bounded(1000) / 100.0f;
        nodos.append(n);
    }
}

void Nivel3::crearFirewalls()
{
    firewalls.clear();
    Firewall f1 = {300,300,30,200,true,80,200,500};
    Firewall f2 = {700,450,30,200,true,-100,250,550};
    Firewall f3 = {500,400,30,150,false,0,0,0};
    firewalls.append(f1);
    firewalls.append(f2);
    firewalls.append(f3);
}

void Nivel3::crearCamposFuerza()
{
    campos.clear();
    CampoFuerza c1 = {terminalX, terminalY + 100, 150, -50};
    campos.append(c1);
    for (int i = 0; i < 2; i++) {
        CampoFuerza c = {200.0f + QRandomGenerator::global()->bounded(624), 
                         300.0f + QRandomGenerator::global()->bounded(300), 100, 80};
        campos.append(c);
    }
}

void Nivel3::generarParticulas()
{
    particulas.clear();
    const char chars[] = "01001101010011100101";
    for (int i = 0; i < 50; i++) {
        Particula p;
        p.x = QRandomGenerator::global()->bounded(1024);
        p.y = QRandomGenerator::global()->bounded(768);
        p.velocidadY = 20.0f + QRandomGenerator::global()->bounded(60);
        p.caracter = chars[QRandomGenerator::global()->bounded(20)];
        p.alpha = 0.2f + QRandomGenerator::global()->bounded(100) / 200.0f;
        particulas.append(p);
    }
}

float Nivel3::distancia(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1, dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}

void Nivel3::actualizar(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    if (!juegoActivo) return;
    
    tiempoRestante -= deltaTime;
    if (tiempoRestante <= 0 || energia <= 0) { finalizarJuego(false); return; }
    if (fragmentosEntregados >= FRAGMENTOS_PARA_GANAR) { finalizarJuego(true); return; }
    
    actualizarFisicaJugador(arriba, abajo, izquierda, derecha, deltaTime);
    aplicarCamposFuerza(deltaTime);
    actualizarVirus(deltaTime);
    actualizarFragmentos(deltaTime);  // NUEVO - Actualizar movimiento de fragmentos
    actualizarNodos(deltaTime);
    actualizarFirewalls(deltaTime);
    actualizarGuardian(deltaTime);
    actualizarParticulas(deltaTime);
    
    verificarColisionesVirus();
    verificarColisionesFirewall();
    verificarColisionGuardian();  // NUEVO - Verificar colisión con guardián
    verificarNodos();
    
    offsetGrid += deltaTime * 50;
    if (offsetGrid > 50) offsetGrid -= 50;
    pulsoTerminal += deltaTime * 3;
    guardianPulso += deltaTime * 2;
    
    if (escudoActivo) {
        tiempoEscudo -= deltaTime;
        if (tiempoEscudo <= 0) { escudoActivo = false; tiempoEscudo = 0; }
    }
    if (tiempoInvulnerable > 0) tiempoInvulnerable -= deltaTime;
}

void Nivel3::actualizarFisicaJugador(bool arriba, bool abajo, bool izquierda, bool derecha, float deltaTime)
{
    if (!jugador) return;
    
    if (arriba) velocidadJugadorY -= ACELERACION * deltaTime;
    if (abajo) velocidadJugadorY += ACELERACION * deltaTime;
    if (izquierda) velocidadJugadorX -= ACELERACION * deltaTime;
    if (derecha) velocidadJugadorX += ACELERACION * deltaTime;
    
    velocidadJugadorX *= FRICCION;
    velocidadJugadorY *= FRICCION;
    
    float vel = sqrt(velocidadJugadorX*velocidadJugadorX + velocidadJugadorY*velocidadJugadorY);
    if (vel > VELOCIDAD_MAXIMA) {
        velocidadJugadorX = (velocidadJugadorX / vel) * VELOCIDAD_MAXIMA;
        velocidadJugadorY = (velocidadJugadorY / vel) * VELOCIDAD_MAXIMA;
    }
    
    float newX = jugador->getPosX() + velocidadJugadorX * deltaTime;
    float newY = jugador->getPosY() + velocidadJugadorY * deltaTime;
    
    if (newX < 30) { newX = 30; velocidadJugadorX *= -COEFICIENTE_RESTITUCION; }
    if (newX > 994) { newX = 994; velocidadJugadorX *= -COEFICIENTE_RESTITUCION; }
    if (newY < 30) { newY = 30; velocidadJugadorY *= -COEFICIENTE_RESTITUCION; }
    if (newY > 738) { newY = 738; velocidadJugadorY *= -COEFICIENTE_RESTITUCION; }
    
    jugador->setPosicion(newX, newY);
    
    if (fragmentoRecogidoIndex >= 0 && fragmentoRecogidoIndex < fragmentos.size()) {
        fragmentos[fragmentoRecogidoIndex].x = newX;
        fragmentos[fragmentoRecogidoIndex].y = newY - 40;
    }
}

void Nivel3::aplicarCamposFuerza(float deltaTime)
{
    if (!jugador) return;
    float px = jugador->getPosX(), py = jugador->getPosY();
    
    for (const CampoFuerza &c : campos) {
        float d = distancia(px, py, c.x, c.y);
        if (d < c.radio && d > 1.0f) {
            float fuerza = c.intensidad * (1.0f - d / c.radio) * deltaTime;
            float dx = (px - c.x) / d;
            float dy = (py - c.y) / d;
            velocidadJugadorX += dx * fuerza;
            velocidadJugadorY += dy * fuerza;
        }
    }
}

void Nivel3::actualizarVirus(float deltaTime)
{
    static float tiempo = 0;
    tiempo += deltaTime;
    
    for (Virus &v : virus) {
        v.fase += deltaTime * v.frecuencia;
        float oscX = sin(v.fase) * v.amplitud;
        float oscY = cos(v.fase * 0.7f) * v.amplitud * 0.5f;
        
        v.x += (v.velocidadBaseX + oscX * 2) * deltaTime;
        v.y += (v.velocidadBaseY + oscY * 2) * deltaTime;
        
        if (v.x < 50 || v.x > 974) v.velocidadBaseX *= -1;
        if (v.y < 150 || v.y > 700) v.velocidadBaseY *= -1;
        
        v.x = qBound(50.0f, v.x, 974.0f);
        v.y = qBound(150.0f, v.y, 700.0f);
    }
}

void Nivel3::actualizarFragmentos(float deltaTime)
{
    // Actualizar movimiento de fragmentos (similar a virus pero más lento)
    for (int i = 0; i < fragmentos.size(); i++) {
        Fragmento &f = fragmentos[i];
        
        // Si está recogido o entregado, no mover
        if (f.recogido || f.entregado) continue;
        
        // Actualizar fase de oscilación
        f.oscilacionFase += deltaTime * f.frecuencia;
        
        // Calcular oscilación
        float oscX = sin(f.oscilacionFase) * f.amplitud;
        float oscY = cos(f.oscilacionFase * 0.8f) * f.amplitud * 0.6f;
        
        // Mover fragmento
        f.x += (f.velocidadBaseX + oscX) * deltaTime;
        f.y += (f.velocidadBaseY + oscY) * deltaTime;
        
        // Rebotar en los bordes
        if (f.x < 80 || f.x > 944) {
            f.velocidadBaseX *= -1;
            f.x = qBound(80.0f, f.x, 944.0f);
        }
        if (f.y < 180 || f.y > 680) {
            f.velocidadBaseY *= -1;
            f.y = qBound(180.0f, f.y, 680.0f);
        }
    }
}

void Nivel3::actualizarNodos(float deltaTime)
{
    for (Nodo &n : nodos) {
        n.pulso += deltaTime * 2;
        if (!n.activo) {
            n.tiempoRecarga -= deltaTime;
            if (n.tiempoRecarga <= 0) n.activo = true;
        }
    }
}

void Nivel3::actualizarFirewalls(float deltaTime)
{
    for (Firewall &f : firewalls) {
        if (f.movil) {
            f.y += f.velocidadY * deltaTime;
            if (f.y < f.limiteInferior || f.y > f.limiteSuperior) {
                f.velocidadY *= -1;
                f.y = qBound(f.limiteInferior, f.y, f.limiteSuperior);
            }
        }
    }
}

void Nivel3::actualizarGuardian(float deltaTime)
{
    if (!guardianActivo || !jugador) return;
    
    float px = jugador->getPosX(), py = jugador->getPosY();
    float dx = px - guardianX, dy = py - guardianY;
    float d = sqrt(dx*dx + dy*dy);
    
    // Velocidad del guardián según dificultad
    int dificultad = DifficultyManager::getInstance().getDificultad();
    float velocidadGuardian;
    float aceleracionGuardian;
    
    switch (dificultad) {
        case 0:  // FÁCIL - Guardián muy lento
            velocidadGuardian = 120.0f;
            aceleracionGuardian = 200.0f;
            break;
        case 1:  // NORMAL - Guardián moderado
            velocidadGuardian = 150.0f;
            aceleracionGuardian = 240.0f;
            break;
        case 2:  // DIFÍCIL - Guardián rápido y agresivo
        default:
            velocidadGuardian = 180.0f;
            aceleracionGuardian = 280.0f;
            break;
    }
    
    // AHORA ES ENEMIGO - PERSIGUE directamente al jugador
    if (d > 1.0f) {
        // Aceleración hacia el jugador
        guardianVelX += (dx / d) * aceleracionGuardian * deltaTime;
        guardianVelY += (dy / d) * aceleracionGuardian * deltaTime;
    }
    
    // Limitar velocidad máxima según dificultad
    float velActual = sqrt(guardianVelX * guardianVelX + guardianVelY * guardianVelY);
    if (velActual > velocidadGuardian) {
        guardianVelX = (guardianVelX / velActual) * velocidadGuardian;
        guardianVelY = (guardianVelY / velActual) * velocidadGuardian;
    }
    
    // Fricción - mayor en fácil para que sea más predecible
    float friccion = (dificultad == 0) ? 0.95f : 0.98f;
    guardianVelX *= friccion;
    guardianVelY *= friccion;
    
    guardianX += guardianVelX * deltaTime;
    guardianY += guardianVelY * deltaTime;
    guardianX = qBound(50.0f, guardianX, 974.0f);
    guardianY = qBound(100.0f, guardianY, 718.0f);
}

void Nivel3::actualizarParticulas(float deltaTime)
{
    for (Particula &p : particulas) {
        p.y += p.velocidadY * deltaTime;
        if (p.y > 768) {
            p.y = 0;
            p.x = QRandomGenerator::global()->bounded(1024);
        }
    }
}

void Nivel3::verificarColisionesVirus()
{
    if (!jugador || tiempoInvulnerable > 0 || escudoActivo) return;
    
    float px = jugador->getPosX(), py = jugador->getPosY();
    for (const Virus &v : virus) {
        if (distancia(px, py, v.x, v.y) < v.radio + 25) {
            energia -= DANIO_VIRUS;
            tiempoInvulnerable = 1.5f;
            velocidadJugadorX *= -0.5f;
            velocidadJugadorY *= -0.5f;
            puntuacion -= 10;
            break;
        }
    }
}

void Nivel3::verificarColisionesFirewall()
{
    if (!jugador) return;
    float px = jugador->getPosX(), py = jugador->getPosY();
    
    for (const Firewall &f : firewalls) {
        QRectF fr(f.x - f.ancho/2, f.y - f.alto/2, f.ancho, f.alto);
        QRectF pr(px - 25, py - 25, 50, 50);
        
        if (fr.intersects(pr)) {
            if (px < f.x) { 
                jugador->setPosicion(f.x - f.ancho/2 - 26, py);
                velocidadJugadorX *= -COEFICIENTE_RESTITUCION;
            } else {
                jugador->setPosicion(f.x + f.ancho/2 + 26, py);
                velocidadJugadorX *= -COEFICIENTE_RESTITUCION;
            }
        }
    }
}

void Nivel3::verificarColisionGuardian()
{
    // El guardián ahora es ENEMIGO - daña al jugador y le quita el fragmento
    if (!jugador || !guardianActivo || tiempoInvulnerable > 0 || escudoActivo) return;
    
    float px = jugador->getPosX(), py = jugador->getPosY();
    float d = distancia(px, py, guardianX, guardianY);
    
    if (d < GUARDIAN_RADIO + 25) {  // Colisión detectada
        // Quitar energía
        energia -= GUARDIAN_DANIO;
        puntuacion -= 30;
        
        // Si tiene un fragmento, quitárselo
        if (fragmentoRecogidoIndex >= 0 && fragmentoRecogidoIndex < fragmentos.size()) {
            Fragmento &f = fragmentos[fragmentoRecogidoIndex];
            f.recogido = false;
            // Soltar el fragmento en una posición aleatoria cercana
            f.x = px + (QRandomGenerator::global()->bounded(200) - 100);
            f.y = py + (QRandomGenerator::global()->bounded(200) - 100);
            f.x = qBound(100.0f, f.x, 924.0f);
            f.y = qBound(200.0f, f.y, 650.0f);
            fragmentoRecogidoIndex = -1;
            qDebug() << "[Nivel3] ¡Guardián robó el fragmento!";
        }
        
        // Empujar al jugador
        float dx = px - guardianX;
        float dy = py - guardianY;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist > 0) {
            velocidadJugadorX = (dx / dist) * 300;
            velocidadJugadorY = (dy / dist) * 300;
        }
        
        tiempoInvulnerable = 2.0f;  // 2 segundos de invulnerabilidad
        qDebug() << "[Nivel3] ¡Guardián atacó al jugador! Energía:" << energia;
    }
}

void Nivel3::verificarNodos()
{
    if (!jugador) return;
    float px = jugador->getPosX(), py = jugador->getPosY();
    
    for (Nodo &n : nodos) {
        if (n.activo && distancia(px, py, n.x, n.y) < n.radio + 25) {
            energia = qMin(energia + NODO_ENERGIA, energiaMaxima);
            n.activo = false;
            n.tiempoRecarga = NODO_RECARGA_TIEMPO;
            puntuacion += 20;
        }
    }
}

void Nivel3::intentarRecogerDato()
{
    if (!jugador || fragmentoRecogidoIndex >= 0) return;
    float px = jugador->getPosX(), py = jugador->getPosY();
    
    for (int i = 0; i < fragmentos.size(); i++) {
        if (!fragmentos[i].recogido && !fragmentos[i].entregado) {
            if (distancia(px, py, fragmentos[i].x, fragmentos[i].y) < 60) {
                fragmentos[i].recogido = true;
                fragmentoRecogidoIndex = i;
                break;
            }
        }
    }
}

void Nivel3::intentarRecogerDatoConClick(float mouseX, float mouseY)
{
    if (fragmentoRecogidoIndex >= 0) return;
    
    for (int i = 0; i < fragmentos.size(); i++) {
        if (!fragmentos[i].recogido && !fragmentos[i].entregado) {
            if (distancia(mouseX, mouseY, fragmentos[i].x, fragmentos[i].y) < 40) {
                fragmentos[i].recogido = true;
                fragmentoRecogidoIndex = i;
                break;
            }
        }
    }
}

void Nivel3::soltarUltimoDato()
{
    if (fragmentoRecogidoIndex >= 0 && fragmentoRecogidoIndex < fragmentos.size()) {
        fragmentos[fragmentoRecogidoIndex].recogido = false;
        fragmentoRecogidoIndex = -1;
    }
}

void Nivel3::verificarDatoEntregado()
{
    if (!jugador || fragmentoRecogidoIndex < 0) return;
    
    float px = jugador->getPosX(), py = jugador->getPosY();
    if (distancia(px, py, terminalX, terminalY + terminalAlto/2) < 100) {
        fragmentos[fragmentoRecogidoIndex].entregado = true;
        fragmentos[fragmentoRecogidoIndex].recogido = false;
        fragmentoRecogidoIndex = -1;
        fragmentosEntregados++;
        puntuacion += DifficultyManager::getInstance().getPuntosPorLetra() * 2;
    }
}

void Nivel3::intentarInteractuarConAutomata() {}

void Nivel3::activarEscudo()
{
    if (!escudoActivo && energia >= COSTO_ESCUDO) {
        escudoActivo = true;
        tiempoEscudo = DURACION_ESCUDO;
        energia -= COSTO_ESCUDO;
    }
}

void Nivel3::finalizarJuego(bool victoria)
{
    juegoActivo = false;
    if (victoria) {
        DifficultyManager::getInstance().registrarRondaCompletada();
        emit nivelCompletado(puntuacion);
    } else {
        emit nivelFallido();
    }
}

void Nivel3::dibujar(QPainter &painter)
{
    dibujarFondo(painter);
    dibujarGrid(painter);
    dibujarParticulas(painter);
    dibujarCampos(painter);
    dibujarFirewalls(painter);
    dibujarNodos(painter);
    dibujarFragmentos(painter);
    dibujarTerminal(painter);
    dibujarVirus(painter);
    dibujarGuardian(painter);
    dibujarJugador(painter);
    dibujarUI(painter);
}

void Nivel3::dibujarFondo(QPainter &painter)
{
    if (!fondoNivel.isNull()) {
        painter.drawPixmap(0, 0, fondoNivel);
    } else {
        painter.fillRect(0, 0, 1024, 768, QColor(10, 15, 30));
    }
}

void Nivel3::dibujarGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor(0, 100, 150, 30), 1));
    for (int x = 0; x < 1024; x += 50) painter.drawLine(x, 0, x, 768);
    for (int y = (int)offsetGrid; y < 768; y += 50) painter.drawLine(0, y, 1024, y);
}

void Nivel3::dibujarParticulas(QPainter &painter)
{
    painter.setFont(QFont("Courier", 10));
    for (const Particula &p : particulas) {
        painter.setPen(QColor(0, 255, 200, (int)(p.alpha * 255)));
        painter.drawText((int)p.x, (int)p.y, QString(p.caracter));
    }
}

void Nivel3::dibujarCampos(QPainter &painter)
{
    for (const CampoFuerza &c : campos) {
        QColor color = c.intensidad < 0 ? QColor(0, 150, 255, 30) : QColor(255, 100, 0, 30);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(c.x, c.y), c.radio, c.radio);
    }
}

void Nivel3::dibujarFirewalls(QPainter &painter)
{
    for (const Firewall &f : firewalls) {
        if (!spriteFirewall.isNull()) {
            QPixmap fw = spriteFirewall.scaled((int)f.ancho + 20, (int)f.alto, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap((int)(f.x - f.ancho/2 - 10), (int)(f.y - f.alto/2), fw);
        } else {
            painter.setBrush(QColor(255, 150, 0, 180));
            painter.setPen(QPen(QColor(255, 200, 0), 2));
            painter.drawRect((int)(f.x - f.ancho/2), (int)(f.y - f.alto/2), (int)f.ancho, (int)f.alto);
        }
    }
}

void Nivel3::dibujarNodos(QPainter &painter)
{
    for (const Nodo &n : nodos) {
        float escala = 1.0f + sin(n.pulso) * 0.1f;
        int size = (int)(n.radio * 2 * escala);
        
        if (!spriteNodo.isNull() && n.activo) {
            QPixmap nodo = spriteNodo.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap((int)(n.x - size/2), (int)(n.y - size/2), nodo);
        } else {
            QColor c = n.activo ? QColor(0, 255, 255, 150) : QColor(100, 100, 100, 100);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(n.x, n.y), n.radio * escala, n.radio * escala);
        }
    }
}

void Nivel3::dibujarFragmentos(QPainter &painter)
{
    for (int i = 0; i < fragmentos.size(); i++) {
        const Fragmento &f = fragmentos[i];
        if (f.entregado) continue;
        if (f.recogido && i != fragmentoRecogidoIndex) continue;
        
        float osc = sin(f.oscilacionFase + offsetGrid * 0.1f) * 5;
        int size = 50;
        
        if (!spriteFragmento.isNull()) {
            QPixmap frag = spriteFragmento.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap((int)(f.x - size/2), (int)(f.y - size/2 + osc), frag);
        } else {
            painter.setBrush(QColor(0, 255, 150, 200));
            painter.setPen(QPen(QColor(100, 255, 200), 2));
            painter.drawEllipse(QPointF(f.x, f.y + osc), 20, 20);
        }
    }
}

void Nivel3::dibujarTerminal(QPainter &painter)
{
    float escala = 1.0f + sin(pulsoTerminal) * 0.05f;
    int w = (int)(terminalAncho * escala);
    int h = (int)(terminalAlto * escala);
    
    if (!spriteTerminal.isNull()) {
        QPixmap term = spriteTerminal.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap((int)(terminalX - w/2), (int)(terminalY), term);
    } else {
        painter.setBrush(QColor(0, 100, 50, 200));
        painter.setPen(QPen(QColor(0, 255, 100), 3));
        painter.drawRect((int)(terminalX - w/2), (int)terminalY, w, h);
    }
}

void Nivel3::dibujarVirus(QPainter &painter)
{
    for (const Virus &v : virus) {
        int size = (int)(v.radio * 2);
        if (!spriteVirus.isNull()) {
            QPixmap vir = spriteVirus.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap((int)(v.x - size/2), (int)(v.y - size/2), vir);
        } else {
            painter.setBrush(QColor(255, 50, 50, 200));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(v.x, v.y), v.radio, v.radio);
        }
    }
}

void Nivel3::dibujarGuardian(QPainter &painter)
{
    if (!guardianActivo) return;
    
    float escala = 1.0f + sin(guardianPulso) * 0.15f;  // Pulso más agresivo
    int size = (int)(80 * escala);  // Más grande
    
    if (!spriteGuardian.isNull()) {
        QPixmap g = spriteGuardian.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap((int)(guardianX - size/2), (int)(guardianY - size/2), g);
    } else {
        // Color rojo para indicar que es enemigo
        painter.setBrush(QColor(255, 50, 50, 180));  // Rojo en vez de morado
        painter.setPen(QPen(QColor(255, 100, 100), 3));
        painter.drawEllipse(QPointF(guardianX, guardianY), 35 * escala, 35 * escala);
        
        // Ojos amenazantes
        painter.setBrush(Qt::white);
        painter.drawEllipse(QPointF(guardianX - 10, guardianY - 5), 6, 6);
        painter.drawEllipse(QPointF(guardianX + 10, guardianY - 5), 6, 6);
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPointF(guardianX - 10, guardianY - 5), 3, 3);
        painter.drawEllipse(QPointF(guardianX + 10, guardianY - 5), 3, 3);
    }
    
    // Texto de advertencia
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText((int)(guardianX - 30), (int)(guardianY - 50), "GUARDIAN");
}

void Nivel3::dibujarJugador(QPainter &painter)
{
    if (!jugador) return;
    
    float px = jugador->getPosX(), py = jugador->getPosY();
    int size = 60;
    
    QPixmap &sprite = escudoActivo ? spritePacketEscudo : spritePacket;
    
    if (!sprite.isNull()) {
        if (tiempoInvulnerable > 0 && ((int)(tiempoInvulnerable * 10) % 2 == 0)) {
            painter.setOpacity(0.5f);
        }
        QPixmap p = sprite.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap((int)(px - size/2), (int)(py - size/2), p);
        painter.setOpacity(1.0f);
    } else {
        QColor c = escudoActivo ? QColor(0, 200, 255) : QColor(0, 255, 255);
        painter.setBrush(c);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(QPointF(px, py), 25, 25);
    }
}

void Nivel3::dibujarUI(QPainter &painter)
{
    painter.fillRect(0, 0, 1024, 70, QColor(0, 0, 0, 180));
    
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(10, 25, "Fragmentos: " + QString::number(fragmentosEntregados) + "/" + QString::number(FRAGMENTOS_PARA_GANAR));
    
    painter.setPen(tiempoRestante < 20 ? Qt::red : Qt::green);
    painter.drawText(10, 50, "Tiempo: " + QString::number((int)tiempoRestante) + "s");
    
    // Barra de energía
    painter.setPen(Qt::white);
    painter.drawText(200, 25, "Energía:");
    painter.drawRect(270, 12, 150, 18);
    float energiaPct = energia / energiaMaxima;
    QColor energiaColor = energiaPct > 0.5 ? Qt::green : (energiaPct > 0.25 ? Qt::yellow : Qt::red);
    painter.fillRect(271, 13, (int)(148 * energiaPct), 16, energiaColor);
    
    painter.setPen(Qt::cyan);
    painter.drawText(450, 25, "Puntos: " + QString::number(puntuacion));
    
    if (escudoActivo) {
        painter.setPen(QColor(0, 200, 255));
        painter.drawText(600, 25, "ESCUDO ACTIVO: " + QString::number((int)tiempoEscudo + 1) + "s");
    } else if (energia >= COSTO_ESCUDO) {
        painter.setPen(QColor(100, 150, 200));
        painter.drawText(600, 25, "[Q] Activar Escudo");
    }
    
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(150, 150, 150));
    painter.drawText(10, 750, "WASD: Mover | R: Recoger | E: Entregar al Terminal | Q: Escudo | ¡CUIDADO CON EL GUARDIAN!");
    
    if (!juegoActivo) {
        painter.setFont(QFont("Arial", 32, QFont::Bold));
        painter.setPen(fragmentosEntregados >= FRAGMENTOS_PARA_GANAR ? Qt::green : Qt::red);
        QString msg = fragmentosEntregados >= FRAGMENTOS_PARA_GANAR ? "¡VICTORIA!" : "GAME OVER";
        painter.drawText(QRect(0, 350, 1024, 50), Qt::AlignCenter, msg);
    }
}
