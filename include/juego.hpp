#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "personaje.hpp"
#include "enemigo.hpp"
#include "interfazusuario.hpp"
#include "entidades.hpp"
#include "recursos.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

enum class GameState { MENU, PERSONAJES, GALERIA, INSTRUCCIONES, JUGANDO, PANTALLA_NIVEL, VICTORIA, GAMEOVER };

class Juego {
private:
    static const bool AUDIO_ACTIVO = true;
    sf::RenderWindow ventana;
    GameState estadoActual;
    Personaje jugador;
    Enemigo* jefe;
    InterfazUsuario ui;

    std::vector<Balon> balones;
    std::vector<AtaqueEnemigo> ataques;
    std::vector<PowerUp> powerups;

    sf::Texture tBalon, tSilbato, tDinero, tFlecha, tTaquetes, tEspinilleras, tGuantes, tBanda, tAmarilla, tRoja, tCorazon;
    sf::Texture tEnemigos[5];
    sf::Texture tPersonajes[6];
    sf::Texture tEscenarios[5];
    sf::Texture tMenuFondo;
    sf::Texture tInstruccionesFondo;

    // Audio
    sf::Music* musicaFondo;
    sf::Music* musicaMenu;
    bool musicaJuegoActiva;
    sf::Sound *sonidoLanzamiento, *sonidoSilbato, *sonidoEnemigoDerrota, *sonidoDerrota, *sonidoVictoria, *sonidoHover, *sonidoGritoGol;
    sf::Sound* sonidosJugador[6];
    sf::Sound* sonidosGolpeJugador[6];
    sf::Sound* sonidosGolpeJefe[5];
    sf::Sound* sonidosMuerteJefe[5];
    sf::SoundBuffer bufLanzamiento, bufSilbato, bufEnemigoDerrota, bufDerrota, bufGolpeJugador, bufRondaPerdida, bufVictoria;

    // Sprites de escenarios
    sf::Sprite escenarioActual;
    sf::Sprite menuFondo;

    int puntos;
    int nivel;
    float tiempoPoder;
    PowerUpType poderActivo;
    sf::Clock relojFrame;
    float tiempoTotalJuego;
    float tiempoAnimacion;
    float tiempoTransicionNivel;
    float tiempoFlashJefe;
    float tiempoSacudida;
    float tiempoSpawnPowerUp;
    float tiempoDisparoJugador;
    float tiempoGolCambio;
    int ultimoJefeDerrotado;

    std::string nombresEnemigos[5] = {"Katie Itzel", "Gata Ortencia", "Telecomerciales", "Funko Arreola", "Mafia Mayor"};
    std::string nombresPersonajes[6] = {"Chicharron", "Cuau", "Funesmorri", "Gino", "Lugo Sanchez", "Chaqueta Gimenez"};
    int personajeSeleccionadoIdx;
    int opcionMenuHover;
    int personajeHover;

    void iniciarNivel(int nvl) {
        delete jefe;
        jefe = new Enemigo(nombresEnemigos[nvl-1], nvl, tEnemigos[nvl-1]);
        balones.clear();
        ataques.clear();
        powerups.clear();
        jugador.desactivarPowerUps();
        poderActivo = PowerUpType::NINGUNO;
        tiempoSpawnPowerUp = 0.f;
        tiempoDisparoJugador = 0.f;
        tiempoFlashJefe = 0.f;
        tiempoSacudida = 0.f;
        tiempoGolCambio = 0.f;
        reproducirSonido(sonidoSilbato);
    }

    sf::Texture& texturaParaPowerUp(PowerUpType tipo) {
        switch (tipo) {
            case PowerUpType::TAQUETES: return tTaquetes;
            case PowerUpType::ESPINILLERAS: return tEspinilleras;
            case PowerUpType::GUANTES: return tGuantes;
            case PowerUpType::BANDA_CAPITAN: return tBanda;
            case PowerUpType::TARJETA_AMARILLA: return tAmarilla;
            case PowerUpType::TARJETA_ROJA: return tRoja;
            case PowerUpType::VIDA_EXTRA: return tCorazon;
            default: return tTaquetes;
        }
    }

    sf::Texture& texturaProyectilEnemigo() {
        if (nivel == 3 || nivel == 5) return tDinero;
        if (nivel == 4) return tFlecha;
        return tSilbato;
    }

    bool esTarjeta(PowerUpType tipo) const {
        return tipo == PowerUpType::TARJETA_AMARILLA || tipo == PowerUpType::TARJETA_ROJA;
    }

    sf::Color colorPowerUp(PowerUpType tipo) const {
        switch (tipo) {
            case PowerUpType::TAQUETES: return sf::Color(235, 55, 55);
            case PowerUpType::ESPINILLERAS: return sf::Color(75, 190, 255);
            case PowerUpType::GUANTES: return sf::Color(150, 95, 230);
            case PowerUpType::BANDA_CAPITAN: return sf::Color(245, 210, 65);
            case PowerUpType::TARJETA_AMARILLA: return sf::Color(255, 230, 55);
            case PowerUpType::TARJETA_ROJA: return sf::Color(255, 65, 55);
            case PowerUpType::VIDA_EXTRA: return sf::Color(255, 80, 130);
            default: return sf::Color::White;
        }
    }

    void generarPowerUp() {
        PowerUpType tiposNormales[6] = {
            PowerUpType::TAQUETES,
            PowerUpType::ESPINILLERAS,
            PowerUpType::GUANTES,
            PowerUpType::BANDA_CAPITAN,
            PowerUpType::TARJETA_AMARILLA,
            PowerUpType::TARJETA_ROJA
        };
        bool generarCorazon = (std::rand() % 4) == 0;
        PowerUpType randomTipo = generarCorazon ? PowerUpType::VIDA_EXTRA : tiposNormales[std::rand() % 6];
        float posicionesY[4] = {520.f, 455.f, 405.f, 365.f};
        float x = static_cast<float>(std::rand() % 680 + 60);
        float y = posicionesY[std::rand() % 4];
        float duracionVisible = esTarjeta(randomTipo) ? 5.f : (randomTipo == PowerUpType::VIDA_EXTRA ? 8.f : 7.f);

        powerups.clear();
        powerups.push_back(PowerUp(x, y, randomTipo, texturaParaPowerUp(randomTipo), 0.f, duracionVisible));
    }

    void dibujarFondoMenu() {
        ventana.clear(sf::Color(5, 14, 28));

        for (int i = 0; i < 14; ++i) {
            sf::RectangleShape banda(sf::Vector2f(800.f, 46.f));
            banda.setPosition(0.f, i * 44.f);
            sf::Uint8 azul = static_cast<sf::Uint8>(24 + i * 3);
            banda.setFillColor(i % 2 == 0 ? sf::Color(6, 24, azul) : sf::Color(8, 36, 42));
            ventana.draw(banda);
        }

        sf::RectangleShape cieloBrillo(sf::Vector2f(800.f, 210.f));
        cieloBrillo.setPosition(0.f, 0.f);
        cieloBrillo.setFillColor(sf::Color(12, 72, 90, 80));
        ventana.draw(cieloBrillo);

        sf::ConvexShape cancha;
        cancha.setPointCount(4);
        cancha.setPoint(0, sf::Vector2f(-70.f, 600.f));
        cancha.setPoint(1, sf::Vector2f(870.f, 600.f));
        cancha.setPoint(2, sf::Vector2f(620.f, 255.f));
        cancha.setPoint(3, sf::Vector2f(180.f, 255.f));
        cancha.setFillColor(sf::Color(18, 135, 66));
        ventana.draw(cancha);

        for (int i = 0; i < 9; ++i) {
            sf::ConvexShape franja;
            float y1 = 600.f - i * 42.f;
            float y2 = y1 - 30.f;
            float estrecha1 = i * 28.f;
            float estrecha2 = (i + 1) * 28.f;
            franja.setPointCount(4);
            franja.setPoint(0, sf::Vector2f(-70.f + estrecha1, y1));
            franja.setPoint(1, sf::Vector2f(870.f - estrecha1, y1));
            franja.setPoint(2, sf::Vector2f(850.f - estrecha2, y2));
            franja.setPoint(3, sf::Vector2f(-50.f + estrecha2, y2));
            franja.setFillColor(i % 2 == 0 ? sf::Color(22, 152, 72, 170) : sf::Color(12, 105, 58, 160));
            ventana.draw(franja);
        }

        sf::CircleShape centro(86.f);
        centro.setOrigin(86.f, 86.f);
        centro.setPosition(400.f, 430.f);
        centro.setScale(1.55f, 0.48f);
        centro.setFillColor(sf::Color::Transparent);
        centro.setOutlineColor(sf::Color(235, 255, 235, 115));
        centro.setOutlineThickness(3.f);
        ventana.draw(centro);

        sf::RectangleShape lineaMedio(sf::Vector2f(560.f, 3.f));
        lineaMedio.setOrigin(280.f, 1.5f);
        lineaMedio.setPosition(400.f, 430.f);
        lineaMedio.setFillColor(sf::Color(235, 255, 235, 100));
        ventana.draw(lineaMedio);

        for (int i = 0; i < 8; ++i) {
            float x = -20.f + i * 120.f;
            sf::RectangleShape grada(sf::Vector2f(95.f, 22.f));
            grada.setPosition(x, 215.f + (i % 2) * 7.f);
            grada.setFillColor(sf::Color(245, 210, 65, 55));
            ventana.draw(grada);
        }

        sf::RectangleShape sombra(sf::Vector2f(800.f, 600.f));
        sombra.setFillColor(sf::Color(0, 0, 0, 30));
        ventana.draw(sombra);
    }

    void dibujarTextoSombra(const std::string& texto, float x, float y, int tam, sf::Color color) {
        ui.dibujarTexto(ventana, texto, x + 3.f, y + 3.f, tam, sf::Color(0, 0, 0, 190));
        ui.dibujarTexto(ventana, texto, x, y, tam, color);
    }

    sf::FloatRect rectBotonMenu(int opcion) {
        switch (opcion) {
            case 0: return sf::FloatRect(292.f, 268.f, 230.f, 116.f);
            case 1: return sf::FloatRect(25.f, 285.f, 170.f, 72.f);
            case 2: return sf::FloatRect(25.f, 363.f, 170.f, 68.f);
            case 3: return sf::FloatRect(25.f, 436.f, 170.f, 72.f);
            default: return sf::FloatRect();
        }
    }

    sf::FloatRect rectPersonaje(int idx) {
        int col = idx % 3;
        int row = idx / 3;
        return sf::FloatRect(75.f + col * 225.f, 160.f + row * 185.f, 190.f, 170.f);
    }

    int opcionMenuEn(sf::Vector2f punto) {
        if (sf::FloatRect(25.f, 208.f, 170.f, 72.f).contains(punto)) return 0;
        for (int i = 0; i < 4; ++i) {
            if (rectBotonMenu(i).contains(punto)) return i;
        }
        return -1;
    }

    int personajeEn(sf::Vector2f punto) {
        for (int i = 0; i < 6; ++i) {
            if (rectPersonaje(i).contains(punto)) return i;
        }
        return -1;
    }

    int columnasPersonaje(int idx) {
        if (idx == 3) return 3;
        if (idx == 5) return 8;
        return 4;
    }

    sf::Sound* crearSonido(sf::SoundBuffer& buffer, float volumen, float tono = 1.f) {
        if (!AUDIO_ACTIVO) return nullptr;

        sf::Sound* sonido = new sf::Sound();
        sonido->setBuffer(buffer);
        sonido->setVolume(volumen);
        sonido->setPitch(tono);
        return sonido;
    }

    void reproducirSonido(sf::Sound* sonido) {
        if (AUDIO_ACTIVO && sonido) sonido->play();
    }

    void reproducirHoverSiCambio(int nuevoHover, int& hoverActual) {
        if (nuevoHover != hoverActual) {
            hoverActual = nuevoHover;
            if (nuevoHover >= 0) reproducirSonido(sonidoHover);
        }
    }

    void actualizarHover(sf::Vector2f punto) {
        if (estadoActual == GameState::MENU) {
            int nuevoPersonajeHover = -1;
            int nuevoOpcionHover = opcionMenuEn(punto);
            reproducirHoverSiCambio(nuevoOpcionHover, opcionMenuHover);
            reproducirHoverSiCambio(nuevoPersonajeHover, personajeHover);
        } else if (estadoActual == GameState::PERSONAJES) {
            int nuevoPersonajeHover = personajeEn(punto);
            reproducirHoverSiCambio(-1, opcionMenuHover);
            reproducirHoverSiCambio(nuevoPersonajeHover, personajeHover);
        } else {
            opcionMenuHover = -1;
            personajeHover = -1;
        }
    }

    void iniciarPartidaConPersonaje(int idx) {
        personajeSeleccionadoIdx = idx;
        jugador.textura = Recursos::cargarTexturaPersonaje(personajeSeleccionadoIdx);
        jugador.x = 400.f;
        jugador.y = 520.f;
        jugador.velocidadY = 0.f;
        jugador.enElSuelo = true;
        jugador.desactivarPowerUps();
        jugador.configurarSprite(columnasPersonaje(personajeSeleccionadoIdx));

        nivel = 1;
        puntos = 0;
        jugador.vidas = 3;
        tiempoTotalJuego = 0.f;
        tiempoDisparoJugador = 0.f;
        iniciarNivel(nivel);
        estadoActual = GameState::JUGANDO;
        opcionMenuHover = -1;
        personajeHover = -1;
    }

    void intentarLanzarBalon() {
        if (tiempoDisparoJugador > 0.f || balones.size() >= 3) return;

        jugador.lanzarBalon(balones, tBalon);
        reproducirSonido(sonidosJugador[personajeSeleccionadoIdx]);
        tiempoDisparoJugador = jugador.disparoTriple ? 0.55f : 0.34f;
    }

    void ejecutarOpcionMenu(int opcion) {
        if (opcion == 0) estadoActual = GameState::PERSONAJES;
        else if (opcion == 1) estadoActual = GameState::GALERIA;
        else if (opcion == 2) estadoActual = GameState::INSTRUCCIONES;
        else if (opcion == 3) ventana.close();
        opcionMenuHover = -1;
        personajeHover = -1;
    }

    void dibujarBotonMenu(int opcion, const std::string& texto, float y, sf::Color color) {
        (void)y;
        bool hover = opcionMenuHover == opcion;
        sf::FloatRect rect = rectBotonMenu(opcion);
        float latido = (std::sin(tiempoAnimacion * 6.f + opcion) + 1.f) * 0.5f;

        sf::RectangleShape sombra(sf::Vector2f(rect.width, rect.height));
        sombra.setPosition(rect.left + 9.f, rect.top + 10.f);
        sombra.setFillColor(sf::Color(0, 0, 0, 135));
        ventana.draw(sombra);

        if (hover) {
            sf::RectangleShape brillo(sf::Vector2f(rect.width + 20.f, rect.height + 16.f));
            brillo.setPosition(rect.left - 10.f, rect.top - 8.f);
            brillo.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(44 + latido * 46)));
            ventana.draw(brillo);
        }

        sf::RectangleShape boton(sf::Vector2f(rect.width, rect.height));
        boton.setPosition(rect.left, rect.top);
        boton.setFillColor(hover ? sf::Color(14, 31, 45, 246) : sf::Color(4, 13, 20, 216));
        boton.setOutlineColor(hover ? sf::Color::White : color);
        boton.setOutlineThickness(hover ? 4.f : 2.f);
        ventana.draw(boton);

        sf::RectangleShape linea(sf::Vector2f(9.f, rect.height));
        linea.setPosition(rect.left, rect.top);
        linea.setFillColor(color);
        ventana.draw(linea);

        sf::CircleShape balon(14.f);
        balon.setPosition(rect.left + 25.f, rect.top + 14.f + (hover ? std::sin(tiempoAnimacion * 10.f) * 2.f : 0.f));
        balon.setFillColor(color);
        balon.setOutlineColor(sf::Color::Black);
        balon.setOutlineThickness(2.f);
        ventana.draw(balon);

        sf::ConvexShape flecha(3);
        float empuje = hover ? 6.f : 0.f;
        flecha.setPoint(0, sf::Vector2f(rect.left + rect.width - 34.f + empuje, rect.top + 18.f));
        flecha.setPoint(1, sf::Vector2f(rect.left + rect.width - 17.f + empuje, rect.top + 28.f));
        flecha.setPoint(2, sf::Vector2f(rect.left + rect.width - 34.f + empuje, rect.top + 38.f));
        flecha.setFillColor(hover ? color : sf::Color(190, 190, 190));
        ventana.draw(flecha);

        ui.dibujarTexto(ventana, texto, rect.left + 68.f, rect.top + 14.f, opcion == 0 ? 22 : 17, hover ? sf::Color(245, 245, 245) : sf::Color::White);
    }

    void dibujarEfectosPortada() {
        for (int i = 0; i < 26; ++i) {
            float x = std::fmod(37.f + i * 73.f + tiempoAnimacion * (18.f + (i % 4) * 8.f), 860.f) - 30.f;
            float y = std::fmod(28.f + i * 41.f + tiempoAnimacion * (26.f + (i % 5) * 7.f), 540.f);
            sf::RectangleShape papel(sf::Vector2f(9.f + (i % 3) * 3.f, 5.f));
            papel.setPosition(x, y);
            papel.setRotation(std::fmod(tiempoAnimacion * 80.f + i * 31.f, 360.f));
            if (i % 4 == 0) papel.setFillColor(sf::Color(245, 210, 65, 170));
            else if (i % 4 == 1) papel.setFillColor(sf::Color(80, 210, 255, 160));
            else if (i % 4 == 2) papel.setFillColor(sf::Color(255, 85, 75, 155));
            else papel.setFillColor(sf::Color(80, 230, 130, 150));
            ventana.draw(papel);
        }

        for (int i = 0; i < 5; ++i) {
            float pulso = (std::sin(tiempoAnimacion * 3.5f + i) + 1.f) * 0.5f;
            sf::CircleShape luz(34.f + pulso * 14.f);
            luz.setOrigin(luz.getRadius(), luz.getRadius());
            luz.setPosition(65.f + i * 165.f, 88.f + std::sin(tiempoAnimacion * 1.7f + i) * 8.f);
            luz.setFillColor(sf::Color(255, 235, 120, static_cast<sf::Uint8>(28 + pulso * 55)));
            ventana.draw(luz);
        }
    }

    void dibujarResumenPortada() {
        sf::RectangleShape marcador(sf::Vector2f(410.f, 58.f));
        marcador.setPosition(44.f, 505.f);
        marcador.setFillColor(sf::Color(3, 11, 18, 222));
        marcador.setOutlineColor(sf::Color(245, 210, 65, 230));
        marcador.setOutlineThickness(2.f);
        ventana.draw(marcador);

        ui.dibujarTexto(ventana, "5 NIVELES", 78.f, 516.f, 17, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "5 RIVALES", 205.f, 516.f, 17, sf::Color(80, 210, 255));
        ui.dibujarTexto(ventana, "100 PTS", 330.f, 516.f, 17, sf::Color(110, 245, 150));
        ui.dibujarTexto(ventana, "Elige PLAY y arma tu futbolista.", 83.f, 543.f, 13, sf::Color(230, 230, 230));
    }

    void dibujarPersonajeMenu(int idx, float x, float y, float alto, bool espejo) {
        if (idx < 0 || idx >= 6 || tPersonajes[idx].getSize().x == 0 || tPersonajes[idx].getSize().y == 0) return;

        int columnas = columnasPersonaje(idx);
        int frameW = static_cast<int>(tPersonajes[idx].getSize().x) / columnas;
        int frameH = static_cast<int>(tPersonajes[idx].getSize().y);
        int frame = static_cast<int>(tiempoAnimacion * 7.f + idx) % columnas;

        sf::CircleShape sombra(40.f);
        sombra.setOrigin(40.f, 12.f);
        sombra.setScale(1.55f, 0.28f);
        sombra.setPosition(x, y + 8.f);
        sombra.setFillColor(sf::Color(0, 0, 0, 130));
        ventana.draw(sombra);

        sf::Sprite sprite(tPersonajes[idx]);
        sprite.setTextureRect(sf::IntRect(frame * frameW, 0, frameW, frameH));
        sprite.setOrigin(frameW / 2.f, frameH * 0.92f);
        float escala = alto / frameH;
        sprite.setScale(espejo ? -escala : escala, escala);
        sprite.setPosition(x, y + std::sin(tiempoAnimacion * 3.f + idx) * 3.f);
        ventana.draw(sprite);
    }

    void dibujarInstruccionesPortada() {
        sf::RectangleShape caja(sf::Vector2f(380.f, 112.f));
        caja.setPosition(58.f, 268.f);
        caja.setFillColor(sf::Color(8, 20, 18, 210));
        caja.setOutlineColor(sf::Color(110, 245, 150, 170));
        caja.setOutlineThickness(2.f);
        ventana.draw(caja);

        ui.dibujarTexto(ventana, "INSTRUCCIONES", 76.f, 282.f, 18, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "1-6 / click: elegir futbolista", 76.f, 310.f, 14, sf::Color::White);
        ui.dibujarTexto(ventana, "Flechas: moverte  |  W/Arriba: saltar", 76.f, 334.f, 14, sf::Color::White);
        ui.dibujarTexto(ventana, "Espacio: lanzar balon", 76.f, 358.f, 14, sf::Color::White);
    }

    void dibujarSelectorPersonajesPortada() {
        sf::RectangleShape panel(sf::Vector2f(292.f, 276.f));
        panel.setPosition(474.f, 70.f);
        panel.setFillColor(sf::Color(0, 0, 0, 155));
        panel.setOutlineColor(sf::Color(245, 210, 65, 190));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        dibujarTextoSombra("ELIGE TU FUTBOLISTA", 492.f, 88.f, 23, sf::Color::White);
        ui.dibujarTexto(ventana, "Selecciona desde la portada para iniciar.", 492.f, 116.f, 12, sf::Color(210, 240, 220));

        for(int i = 0; i < 6; ++i) {
            sf::FloatRect rect = rectPersonaje(i);
            bool hover = personajeHover == i;

            sf::RectangleShape sombra(sf::Vector2f(rect.width, rect.height));
            sombra.setPosition(rect.left + 4.f, rect.top + 5.f);
            sombra.setFillColor(sf::Color(0, 0, 0, 130));
            ventana.draw(sombra);

            sf::RectangleShape card(sf::Vector2f(rect.width, rect.height));
            card.setPosition(rect.left, rect.top);
            card.setFillColor(hover ? sf::Color(28, 58, 45, 245) : sf::Color(15, 28, 28, 225));
            card.setOutlineColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
            card.setOutlineThickness(hover ? 3.f : 2.f);
            ventana.draw(card);

            sf::CircleShape numero(12.f);
            numero.setPosition(rect.left + 9.f, rect.top + 8.f);
            numero.setFillColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
            numero.setOutlineColor(sf::Color::Black);
            numero.setOutlineThickness(2.f);
            ventana.draw(numero);

            ui.dibujarTexto(ventana, std::to_string(i + 1), rect.left + 16.f, rect.top + 10.f, 13, sf::Color::Black);
            ui.dibujarTexto(ventana, nombresPersonajes[i], rect.left + 38.f, rect.top + 12.f, 11, sf::Color::White);
        }
    }

    void dibujarItemsPortada() {
        sf::RectangleShape panel(sf::Vector2f(292.f, 188.f));
        panel.setPosition(474.f, 360.f);
        panel.setFillColor(sf::Color(0, 0, 0, 155));
        panel.setOutlineColor(sf::Color(110, 245, 150, 170));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        ui.dibujarTexto(ventana, "ITEMS", 492.f, 374.f, 18, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Taquetes: mas velocidad", 492.f, 404.f, 12, sf::Color::White);
        ui.dibujarTexto(ventana, "Espinilleras: invencible", 492.f, 426.f, 12, sf::Color::White);
        ui.dibujarTexto(ventana, "Guantes: invencible 10s", 492.f, 448.f, 12, sf::Color::White);
        ui.dibujarTexto(ventana, "Banda: triple tiro + poder", 492.f, 470.f, 12, sf::Color::White);
        ui.dibujarTexto(ventana, "Amarilla: te congela", 492.f, 492.f, 12, sf::Color(255, 235, 95));
        ui.dibujarTexto(ventana, "Roja: pierdes la partida", 492.f, 514.f, 12, sf::Color(255, 100, 90));
    }

    void dibujarPantallaPersonajes() {
        dibujarFondoMenu();

        sf::RectangleShape panel(sf::Vector2f(700.f, 500.f));
        panel.setPosition(50.f, 55.f);
        panel.setFillColor(sf::Color(0, 0, 0, 150));
        panel.setOutlineColor(sf::Color(245, 210, 65));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        dibujarTextoSombra("ELIGE TU PERSONAJE", 185.f, 80.f, 38, sf::Color::White);

        for(int i = 0; i < 6; ++i) {
            sf::FloatRect rect = rectPersonaje(i);
            bool hover = personajeHover == i;

            sf::CircleShape foco(54.f);
            foco.setOrigin(54.f, 54.f);
            foco.setScale(1.2f, 0.58f);
            foco.setPosition(rect.left + rect.width * 0.5f, rect.top + 86.f);
            foco.setFillColor(hover ? sf::Color(245, 210, 65, 72) : sf::Color(80, 210, 255, 32));
            ventana.draw(foco);

            if (hover) {
                sf::CircleShape seleccionado(7.f);
                seleccionado.setOrigin(7.f, 7.f);
                seleccionado.setPosition(rect.left + rect.width * 0.5f, rect.top + 8.f + std::sin(tiempoAnimacion * 8.f) * 3.f);
                seleccionado.setFillColor(sf::Color(245, 210, 65));
                ventana.draw(seleccionado);
            }

            sf::Sprite preview(tPersonajes[i]);
            int columnas = columnasPersonaje(i);
            if (columnas > 0) {
                int frameW = static_cast<int>(tPersonajes[i].getSize().x) / columnas;
                int frameH = static_cast<int>(tPersonajes[i].getSize().y);
                preview.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
            }
            sf::FloatRect bounds = preview.getLocalBounds();
            if (bounds.width > 0.f && bounds.height > 0.f) {
                preview.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                float escala = std::min(118.f / bounds.width, 142.f / bounds.height);
                if (hover) escala *= 1.08f;
                preview.setScale(escala, escala);
                preview.setPosition(rect.left + rect.width * 0.5f, rect.top + 82.f);

                sf::CircleShape resplandor(38.f);
                resplandor.setOrigin(38.f, 38.f);
                resplandor.setScale(1.05f, 0.95f);
                resplandor.setPosition(rect.left + 58.f, rect.top + 58.f);
                resplandor.setFillColor(sf::Color(170, 235, 205, 32));
                ventana.draw(resplandor);

                sf::CircleShape sombra(29.f);
                sombra.setOrigin(29.f, 9.f);
                sombra.setScale(1.75f, 0.28f);
                sombra.setPosition(rect.left + rect.width * 0.5f, rect.top + 132.f);
                sombra.setFillColor(sf::Color(0, 0, 0, 135));
                ventana.draw(sombra);

                ventana.draw(preview);
            }

            sf::RectangleShape nombre(sf::Vector2f(170.f, 30.f));
            nombre.setOrigin(85.f, 0.f);
            nombre.setPosition(rect.left + rect.width * 0.5f, rect.top + 143.f);
            nombre.setFillColor(hover ? sf::Color(245, 210, 65, 230) : sf::Color(4, 18, 42, 210));
            nombre.setOutlineColor(hover ? sf::Color::White : sf::Color(80, 210, 255, 120));
            nombre.setOutlineThickness(2.f);
            ventana.draw(nombre);

            ui.dibujarTexto(ventana, nombresPersonajes[i], rect.left + 28.f, rect.top + 148.f, 13, hover ? sf::Color::Black : sf::Color::White);
        }

        ui.dibujarTexto(ventana, "ENTER / ESC para regresar", 285.f, 522.f, 18, sf::Color(245, 210, 65));
    }

    void dibujarPantallaInstrucciones() {
        ventana.clear(sf::Color(4, 10, 24));

        if (tInstruccionesFondo.getSize().x > 0 && tInstruccionesFondo.getSize().y > 0) {
            sf::Sprite instrucciones(tInstruccionesFondo);
            float escala = std::min(
                800.f / instrucciones.getLocalBounds().width,
                600.f / instrucciones.getLocalBounds().height
            );
            instrucciones.setScale(escala, escala);
            instrucciones.setPosition(
                (800.f - instrucciones.getLocalBounds().width * escala) * 0.5f,
                (600.f - instrucciones.getLocalBounds().height * escala) * 0.5f
            );
            ventana.draw(instrucciones);
        } else {
            dibujarFondoMenu();
            dibujarTextoSombra("INSTRUCCIONES", 245.f, 85.f, 38, sf::Color::White);
        }

        sf::RectangleShape regresar(sf::Vector2f(360.f, 34.f));
        regresar.setPosition(220.f, 557.f);
        regresar.setFillColor(sf::Color(4, 18, 42, 210));
        regresar.setOutlineColor(sf::Color(245, 210, 65));
        regresar.setOutlineThickness(2.f);
        ventana.draw(regresar);
        ui.dibujarTexto(ventana, "ENTER / ESC / CLICK para regresar", 245.f, 564.f, 15, sf::Color(245, 210, 65));
    }

    void dibujarLucesEstadio(float baseY) {
        for (int i = 0; i < 8; ++i) {
            float x = 70.f + i * 95.f;
            float pulso = (std::sin(tiempoAnimacion * 4.f + i * 0.7f) + 1.f) * 0.5f;
            sf::CircleShape luz(18.f + pulso * 6.f);
            luz.setOrigin(luz.getRadius(), luz.getRadius());
            luz.setPosition(x, baseY + std::sin(tiempoAnimacion * 2.f + i) * 8.f);
            luz.setFillColor(sf::Color(245, 230, 120, static_cast<sf::Uint8>(70 + pulso * 110)));
            ventana.draw(luz);
        }
    }

    void dibujarTransicionNivel() {
        if (nivel >= 1 && nivel <= 5 && tEscenarios[nivel - 1].getSize().x > 0 && tEscenarios[nivel - 1].getSize().y > 0) {
            sf::Sprite fondo(tEscenarios[nivel - 1]);
            float sx = 800.f / tEscenarios[nivel - 1].getSize().x;
            float sy = 600.f / tEscenarios[nivel - 1].getSize().y;
            float escala = std::max(sx, sy);
            fondo.setScale(escala, escala);
            fondo.setPosition((800.f - tEscenarios[nivel - 1].getSize().x * escala) * 0.5f,
                              (600.f - tEscenarios[nivel - 1].getSize().y * escala) * 0.5f);
            ventana.draw(fondo);
        } else {
            dibujarFondoMenu();
        }

        sf::RectangleShape capa(sf::Vector2f(800.f, 600.f));
        capa.setFillColor(sf::Color(0, 0, 0, 105));
        ventana.draw(capa);

        float pulso = (std::sin(tiempoAnimacion * 6.5f) + 1.f) * 0.5f;
        unsigned int tamGol = static_cast<unsigned int>(82.f + pulso * 34.f);
        float xGol = 400.f - (tamGol * 2.05f);
        float yGol = 220.f - (tamGol * 0.55f);

        dibujarTextoSombra("GOOOOL", xGol, yGol, tamGol, sf::Color(255, 238, 70));
        ui.dibujarTexto(ventana, "Avanzando a nivel " + std::to_string(nivel), 285.f, 336.f, 26, sf::Color::White);
    }

    void dibujarFlashGolpeJefe() {
        if (tiempoFlashJefe <= 0.f || !jefe) return;

        float alpha = std::min(170.f, tiempoFlashJefe * 850.f);
        sf::CircleShape impacto(58.f + (0.18f - tiempoFlashJefe) * 130.f);
        impacto.setOrigin(impacto.getRadius(), impacto.getRadius());
        impacto.setPosition(jefe->sprite.getPosition());
        impacto.setFillColor(sf::Color(255, 230, 80, static_cast<sf::Uint8>(alpha)));
        impacto.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        impacto.setOutlineThickness(3.f);
        ventana.draw(impacto);
    }

    void dibujarGolCambioEnemigo() {
        if (tiempoGolCambio <= 0.f) return;

        float vida = std::min(1.f, tiempoGolCambio / 2.35f);
        float entrada = std::min(1.f, (2.35f - tiempoGolCambio) / 0.35f);
        float pulso = (std::sin(tiempoAnimacion * 9.f) + 1.f) * 0.5f;
        float alpha = std::min(255.f, vida * 300.f);

        sf::RectangleShape capa(sf::Vector2f(800.f, 600.f));
        capa.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(80.f * vida)));
        ventana.draw(capa);

        for (int i = 0; i < 4; ++i) {
            float radio = 58.f + std::fmod((2.35f - tiempoGolCambio) * 260.f + i * 48.f, 230.f);
            float fade = 1.f - std::min(1.f, radio / 270.f);
            sf::CircleShape onda(radio);
            onda.setOrigin(radio, radio);
            onda.setPosition(400.f, 180.f);
            onda.setFillColor(sf::Color::Transparent);
            onda.setOutlineColor(sf::Color(255, 238, 70, static_cast<sf::Uint8>(150.f * fade * vida)));
            onda.setOutlineThickness(4.f);
            ventana.draw(onda);
        }

        for (int i = 0; i < 28; ++i) {
            float x = std::fmod(i * 67.f + tiempoAnimacion * (95.f + (i % 4) * 18.f), 860.f) - 30.f;
            float y = std::fmod(i * 41.f + tiempoAnimacion * (150.f + (i % 5) * 12.f), 360.f) - 40.f;
            sf::RectangleShape papel(sf::Vector2f(11.f + (i % 3) * 3.f, 6.f));
            papel.setOrigin(papel.getSize().x / 2.f, papel.getSize().y / 2.f);
            papel.setPosition(x, y);
            papel.setRotation(std::fmod(tiempoAnimacion * 210.f + i * 27.f, 360.f));
            sf::Color colores[4] = {
                sf::Color(255, 238, 70, static_cast<sf::Uint8>(alpha)),
                sf::Color(80, 210, 255, static_cast<sf::Uint8>(alpha)),
                sf::Color(255, 90, 95, static_cast<sf::Uint8>(alpha)),
                sf::Color(110, 245, 150, static_cast<sf::Uint8>(alpha))
            };
            papel.setFillColor(colores[i % 4]);
            ventana.draw(papel);
        }

        float sacudida = std::sin(tiempoAnimacion * 42.f) * (1.f - entrada) * 8.f;
        unsigned int tam = static_cast<unsigned int>(78.f + pulso * 14.f);
        dibujarTextoSombra("GOOOOL", 206.f + sacudida, 78.f - (1.f - entrada) * 28.f, tam, sf::Color(255, 238, 70, static_cast<sf::Uint8>(alpha)));
        dibujarTextoSombra("NUEVO RIVAL: " + nombresEnemigos[nivel - 1], 210.f, 176.f, 25, sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }

    void dibujarPowerUpConFeedback(const PowerUp& powerup) {
        sf::Sprite item = powerup.sprite;
        float ratio = powerup.vidaNormalizada();
        float pulso = (std::sin(tiempoAnimacion * 8.f) + 1.f) * 0.5f;
        sf::Color color = colorPowerUp(powerup.tipo);
        sf::Vector2f pos = item.getPosition();

        sf::CircleShape halo(30.f + pulso * 5.f);
        halo.setOrigin(halo.getRadius(), halo.getRadius());
        halo.setScale(1.18f, 0.72f);
        halo.setPosition(pos.x, pos.y + 8.f);
        halo.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(35 + pulso * 45)));
        ventana.draw(halo);

        if (powerup.tiempoVida <= 2.f) {
            sf::Uint8 alpha = static_cast<sf::Uint8>(120 + pulso * 135);
            item.setColor(sf::Color(255, 255, 255, alpha));
        }

        ventana.draw(item);

        sf::RectangleShape fondo(sf::Vector2f(42.f, 5.f));
        fondo.setOrigin(21.f, 2.5f);
        fondo.setPosition(pos.x, pos.y + 31.f);
        fondo.setFillColor(sf::Color(0, 0, 0, 165));
        ventana.draw(fondo);

        sf::RectangleShape tiempo(sf::Vector2f(40.f * ratio, 3.f));
        tiempo.setOrigin(20.f, 1.5f);
        tiempo.setPosition(pos.x, pos.y + 31.f);
        tiempo.setFillColor(color);
        ventana.draw(tiempo);

        if (esTarjeta(powerup.tipo)) {
            ui.dibujarTexto(ventana, std::to_string(std::max(1, static_cast<int>(std::ceil(powerup.tiempoVida)))), pos.x - 5.f, pos.y - 38.f, 14, color);
        }
    }

    void dibujarFuegoArtificial(float cx, float cy, float radio, sf::Color color, float desfase) {
        float pulso = 0.75f + std::sin(tiempoAnimacion * 2.6f + desfase) * 0.25f;
        sf::VertexArray rayos(sf::Lines);

        for (int i = 0; i < 18; ++i) {
            float angulo = (6.28318f / 18.f) * i + desfase * 0.08f;
            float interior = radio * 0.18f;
            float exterior = radio * (0.72f + pulso * 0.28f);
            sf::Vector2f a(cx + std::cos(angulo) * interior, cy + std::sin(angulo) * interior);
            sf::Vector2f b(cx + std::cos(angulo) * exterior, cy + std::sin(angulo) * exterior);
            rayos.append(sf::Vertex(a, sf::Color(255, 255, 255, 210)));
            rayos.append(sf::Vertex(b, color));
        }

        ventana.draw(rayos);

        sf::CircleShape centro(5.f + pulso * 3.f);
        centro.setOrigin(centro.getRadius(), centro.getRadius());
        centro.setPosition(cx, cy);
        centro.setFillColor(sf::Color(255, 255, 255, 230));
        ventana.draw(centro);
    }

    void dibujarCopaLigaMx(float x, float y, float escala) {
        sf::ConvexShape copa;
        copa.setPointCount(6);
        copa.setPoint(0, sf::Vector2f(x - 33.f * escala, y - 70.f * escala));
        copa.setPoint(1, sf::Vector2f(x + 33.f * escala, y - 70.f * escala));
        copa.setPoint(2, sf::Vector2f(x + 22.f * escala, y - 14.f * escala));
        copa.setPoint(3, sf::Vector2f(x + 9.f * escala, y + 2.f * escala));
        copa.setPoint(4, sf::Vector2f(x - 9.f * escala, y + 2.f * escala));
        copa.setPoint(5, sf::Vector2f(x - 22.f * escala, y - 14.f * escala));
        copa.setFillColor(sf::Color(248, 204, 65));
        copa.setOutlineColor(sf::Color(95, 60, 12));
        copa.setOutlineThickness(3.f * escala);
        ventana.draw(copa);

        sf::CircleShape asa(24.f * escala);
        asa.setFillColor(sf::Color::Transparent);
        asa.setOutlineColor(sf::Color(248, 204, 65));
        asa.setOutlineThickness(7.f * escala);
        asa.setOrigin(24.f * escala, 24.f * escala);
        asa.setScale(0.72f, 1.f);
        asa.setPosition(x - 39.f * escala, y - 42.f * escala);
        ventana.draw(asa);
        asa.setPosition(x + 39.f * escala, y - 42.f * escala);
        ventana.draw(asa);

        sf::RectangleShape tallo(sf::Vector2f(16.f * escala, 38.f * escala));
        tallo.setOrigin(8.f * escala, 0.f);
        tallo.setPosition(x, y - 1.f * escala);
        tallo.setFillColor(sf::Color(226, 157, 35));
        ventana.draw(tallo);

        sf::RectangleShape base(sf::Vector2f(86.f * escala, 18.f * escala));
        base.setOrigin(43.f * escala, 0.f);
        base.setPosition(x, y + 34.f * escala);
        base.setFillColor(sf::Color(40, 46, 64));
        base.setOutlineColor(sf::Color(12, 18, 30));
        base.setOutlineThickness(2.f * escala);
        ventana.draw(base);

        sf::RectangleShape placa(sf::Vector2f(64.f * escala, 15.f * escala));
        placa.setOrigin(32.f * escala, 0.f);
        placa.setPosition(x, y + 37.f * escala);
        placa.setFillColor(sf::Color(245, 210, 65));
        ventana.draw(placa);
        ui.dibujarTexto(ventana, "LIGA MX", x - 29.f * escala, y + 38.f * escala, static_cast<int>(13.f * escala), sf::Color::Black);
    }

    void dibujarPantallaVictoria() {
        ventana.clear(sf::Color(12, 25, 42));

        sf::RectangleShape cielo(sf::Vector2f(800.f, 230.f));
        cielo.setFillColor(sf::Color(14, 31, 49));
        ventana.draw(cielo);

        for (int fila = 0; fila < 4; ++fila) {
            for (int i = 0; i < 18; ++i) {
                float x = 20.f + i * 45.f + (fila % 2) * 18.f;
                float y = 50.f + fila * 43.f;
                sf::CircleShape cabeza(8.f);
                cabeza.setPosition(x, y);
                cabeza.setFillColor(sf::Color(90, 78, 72, 160));
                ventana.draw(cabeza);

                sf::RectangleShape cuerpo(sf::Vector2f(18.f, 16.f));
                cuerpo.setPosition(x - 1.f, y + 15.f);
                cuerpo.setFillColor(i % 3 == 0 ? sf::Color(24, 49, 84, 150) : sf::Color(58, 70, 82, 145));
                ventana.draw(cuerpo);
            }
        }

        sf::RectangleShape grada(sf::Vector2f(800.f, 42.f));
        grada.setPosition(0.f, 174.f);
        grada.setFillColor(sf::Color(89, 112, 124, 215));
        ventana.draw(grada);

        sf::RectangleShape barda(sf::Vector2f(800.f, 18.f));
        barda.setPosition(0.f, 216.f);
        barda.setFillColor(sf::Color(28, 42, 55));
        ventana.draw(barda);

        sf::RectangleShape cancha(sf::Vector2f(800.f, 366.f));
        cancha.setPosition(0.f, 234.f);
        cancha.setFillColor(sf::Color(66, 153, 62));
        ventana.draw(cancha);

        for (int i = 0; i < 8; ++i) {
            sf::RectangleShape franja(sf::Vector2f(120.f, 366.f));
            franja.setPosition(i * 110.f - 25.f, 234.f);
            franja.setFillColor(i % 2 == 0 ? sf::Color(78, 171, 70, 90) : sf::Color(40, 120, 54, 80));
            ventana.draw(franja);
        }

        sf::CircleShape circuloMedio(158.f);
        circuloMedio.setOrigin(158.f, 158.f);
        circuloMedio.setPosition(400.f, 505.f);
        circuloMedio.setFillColor(sf::Color::Transparent);
        circuloMedio.setOutlineColor(sf::Color(230, 245, 225, 210));
        circuloMedio.setOutlineThickness(5.f);
        ventana.draw(circuloMedio);

        sf::RectangleShape linea(sf::Vector2f(5.f, 366.f));
        linea.setPosition(398.f, 234.f);
        linea.setFillColor(sf::Color(230, 245, 225, 195));
        ventana.draw(linea);

        sf::RectangleShape tableroSombra(sf::Vector2f(540.f, 184.f));
        tableroSombra.setPosition(140.f, 20.f);
        tableroSombra.setFillColor(sf::Color(0, 0, 0, 120));
        ventana.draw(tableroSombra);

        sf::RectangleShape tablero(sf::Vector2f(520.f, 170.f));
        tablero.setPosition(140.f, 14.f);
        tablero.setFillColor(sf::Color(8, 22, 27, 238));
        tablero.setOutlineColor(sf::Color(172, 191, 197));
        tablero.setOutlineThickness(5.f);
        ventana.draw(tablero);

        sf::RectangleShape pantallaA(sf::Vector2f(498.f, 56.f));
        pantallaA.setPosition(151.f, 25.f);
        pantallaA.setFillColor(sf::Color(13, 34, 35, 245));
        pantallaA.setOutlineColor(sf::Color(61, 77, 83));
        pantallaA.setOutlineThickness(2.f);
        ventana.draw(pantallaA);

        sf::RectangleShape pantallaB(sf::Vector2f(498.f, 88.f));
        pantallaB.setPosition(151.f, 90.f);
        pantallaB.setFillColor(sf::Color(13, 34, 35, 245));
        pantallaB.setOutlineColor(sf::Color(61, 77, 83));
        pantallaB.setOutlineThickness(2.f);
        ventana.draw(pantallaB);

        dibujarTextoSombra("FINAL DEL PARTIDO", 218.f, 36.f, 34, sf::Color::White);
        dibujarTextoSombra("GANASTE", 255.f, 101.f, 43, sf::Color(245, 210, 65));
        dibujarTextoSombra("CAMPEON DE LIGA MX", 188.f, 142.f, 31, sf::Color(245, 210, 65));

        auto dibujarPersonajeCelebrando = [&](int idx, float x, float y, float alto, bool espejo, int frame) {
            if (idx < 0 || idx >= 6 || tPersonajes[idx].getSize().x == 0) return;

            int columnas = columnasPersonaje(idx);
            int frameW = static_cast<int>(tPersonajes[idx].getSize().x) / columnas;
            int frameH = static_cast<int>(tPersonajes[idx].getSize().y);
            frame = std::max(0, std::min(columnas - 1, frame));

            sf::Sprite sprite(tPersonajes[idx]);
            sprite.setTextureRect(sf::IntRect(frame * frameW, 0, frameW, frameH));
            sprite.setOrigin(frameW / 2.f, frameH * 0.92f);
            float escala = alto / static_cast<float>(frameH);
            sprite.setScale(espejo ? -escala : escala, escala);
            sprite.setPosition(x, y);
            ventana.draw(sprite);
        };

        float brinco = std::sin(tiempoAnimacion * 5.5f) * 7.f;
        dibujarPersonajeCelebrando(personajeSeleccionadoIdx, 82.f, 500.f, 118.f, false, 1);
        dibujarPersonajeCelebrando(personajeSeleccionadoIdx, 710.f, 500.f, 118.f, true, 1);

        sf::CircleShape sombraCentral(76.f);
        sombraCentral.setOrigin(76.f, 22.f);
        sombraCentral.setScale(1.35f, 0.34f);
        sombraCentral.setPosition(400.f, 536.f);
        sombraCentral.setFillColor(sf::Color(0, 0, 0, 115));
        ventana.draw(sombraCentral);

        dibujarPersonajeCelebrando(personajeSeleccionadoIdx, 400.f, 536.f + brinco, 245.f, false, 0);
        dibujarCopaLigaMx(400.f, 286.f + brinco, 0.9f);

        for (int i = 0; i < 86; ++i) {
            float base = static_cast<float>(i);
            float x = std::fmod(base * 61.f + tiempoAnimacion * (38.f + (i % 4) * 11.f), 850.f) - 25.f;
            float y = std::fmod(base * 37.f + tiempoAnimacion * (72.f + (i % 5) * 9.f), 620.f) - 20.f;
            sf::RectangleShape papel(sf::Vector2f(10.f + (i % 3) * 4.f, 5.f));
            papel.setPosition(x, y);
            papel.setRotation(std::fmod(tiempoAnimacion * 120.f + base * 29.f, 360.f));
            if (i % 4 == 0) papel.setFillColor(sf::Color(245, 210, 65, 225));
            else if (i % 4 == 1) papel.setFillColor(sf::Color(255, 255, 255, 230));
            else if (i % 4 == 2) papel.setFillColor(sf::Color(32, 91, 168, 220));
            else papel.setFillColor(sf::Color(110, 245, 150, 205));
            ventana.draw(papel);
        }

        sf::RectangleShape marcador(sf::Vector2f(760.f, 40.f));
        marcador.setPosition(20.f, 552.f);
        marcador.setFillColor(sf::Color(4, 18, 42, 218));
        marcador.setOutlineColor(sf::Color(245, 210, 65));
        marcador.setOutlineThickness(2.f);
        ventana.draw(marcador);
        ui.dibujarTexto(ventana, nombresPersonajes[personajeSeleccionadoIdx], 48.f, 562.f, 15, sf::Color::White);
        ui.dibujarTexto(ventana, "Puntos: " + std::to_string(puntos), 250.f, 562.f, 15, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Tiempo: " + std::to_string((int)tiempoTotalJuego) + "s", 385.f, 562.f, 15, sf::Color::White);
        ui.dibujarTexto(ventana, "ENTER / CLICK para volver al Menu", 520.f, 562.f, 14, sf::Color(110, 245, 150));
    }

    void dibujarPantallaGameOver() {
        ventana.clear(sf::Color(16, 4, 8));

        if (nivel >= 1 && nivel <= 5 && tEscenarios[nivel - 1].getSize().x > 0 && tEscenarios[nivel - 1].getSize().y > 0) {
            sf::Sprite fondo(tEscenarios[nivel - 1]);
            fondo.setScale(800.f / fondo.getLocalBounds().width, 600.f / fondo.getLocalBounds().height);
            ventana.draw(fondo);
        } else {
            dibujarFondoMenu();
        }

        sf::RectangleShape velo(sf::Vector2f(800.f, 600.f));
        velo.setFillColor(sf::Color(25, 0, 0, 155));
        ventana.draw(velo);

        sf::RectangleShape panel(sf::Vector2f(610.f, 310.f));
        panel.setPosition(95.f, 130.f);
        panel.setFillColor(sf::Color(8, 10, 18, 220));
        panel.setOutlineColor(sf::Color(255, 65, 55));
        panel.setOutlineThickness(3.f);
        ventana.draw(panel);

        dibujarTextoSombra("GAME OVER", 245.f, 160.f, 54, sf::Color(255, 80, 70));
        ui.dibujarTexto(ventana, "La ultima jugada dolio, pero el estadio sigue esperando.", 155.f, 238.f, 17, sf::Color(230, 230, 230));

        sf::RectangleShape linea(sf::Vector2f(470.f, 2.f));
        linea.setPosition(165.f, 282.f);
        linea.setFillColor(sf::Color(255, 80, 70, 160));
        ventana.draw(linea);

        ui.dibujarTexto(ventana, "Puntaje alcanzado: " + std::to_string(puntos), 232.f, 310.f, 22, sf::Color::White);
        ui.dibujarTexto(ventana, "Nivel alcanzado: " + std::to_string(nivel), 258.f, 350.f, 22, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "ENTER / CLICK para volver al Menu", 218.f, 402.f, 18, sf::Color(110, 245, 150));
    }

    void actualizarMusica() {
        if (!AUDIO_ACTIVO) return;

        bool usarMusicaJuego = estadoActual == GameState::JUGANDO || estadoActual == GameState::PANTALLA_NIVEL;
        sf::Music* musicaActiva = usarMusicaJuego ? musicaFondo : musicaMenu;
        sf::Music* musicaPausada = usarMusicaJuego ? musicaMenu : musicaFondo;

        if (musicaJuegoActiva == usarMusicaJuego && musicaActiva && musicaActiva->getStatus() == sf::Music::Playing) {
            return;
        }

        if (musicaPausada && musicaPausada->getStatus() == sf::Music::Playing) {
            musicaPausada->stop();
        }

        if (musicaActiva && musicaActiva->getStatus() != sf::Music::Playing) {
            musicaActiva->play();
        }

        musicaJuegoActiva = usarMusicaJuego;
    }

public:
    Juego() : ventana(sf::VideoMode(800, 600), "Futbol Adventure: Liga de Barrio"), estadoActual(GameState::MENU), musicaFondo(nullptr), musicaMenu(nullptr), musicaJuegoActiva(false),
              sonidoLanzamiento(nullptr), sonidoSilbato(nullptr), sonidoEnemigoDerrota(nullptr), sonidoDerrota(nullptr), sonidoVictoria(nullptr), sonidoHover(nullptr), sonidoGritoGol(nullptr) {
        ventana.setVisible(true);
        ventana.setPosition(sf::Vector2i(80, 80));
        ventana.setFramerateLimit(60);
        ventana.requestFocus();
        ventana.setKeyRepeatEnabled(false);

        ventana.clear(sf::Color(8, 24, 28));
        ui.dibujarTexto(ventana, "CARGANDO THE LAST DANCE...", 230.f, 260.f, 26, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Preparando estadio y personajes", 255.f, 305.f, 18, sf::Color::White);
        ventana.display();

        std::srand(std::time(nullptr));
        puntos = 0;
        nivel = 1;
        tiempoPoder = 0.f;
        tiempoTotalJuego = 0.f;
        tiempoAnimacion = 0.f;
        tiempoTransicionNivel = 0.f;
        tiempoFlashJefe = 0.f;
        tiempoSacudida = 0.f;
        tiempoSpawnPowerUp = 0.f;
        tiempoDisparoJugador = 0.f;
        tiempoGolCambio = 0.f;
        ultimoJefeDerrotado = 0;
        poderActivo = PowerUpType::NINGUNO;
        personajeSeleccionadoIdx = 0;
        opcionMenuHover = -1;
        personajeHover = -1;
        jefe = nullptr;
        for (int i = 0; i < 6; ++i) {
            sonidosJugador[i] = nullptr;
            sonidosGolpeJugador[i] = nullptr;
        }
        for (int i = 0; i < 5; ++i) {
            sonidosGolpeJefe[i] = nullptr;
            sonidosMuerteJefe[i] = nullptr;
        }

        // Cargar texturas de proyectiles
        tBalon = Recursos::cargarTexturaBalon();
        tSilbato = Recursos::cargarTexturaPowerUp("silbato");
        tDinero = Recursos::cargarTexturaPowerUp("dinero");
        tFlecha = Recursos::cargarTexturaPowerUp("flecha");
        tTaquetes = Recursos::cargarTexturaPowerUp("taquetes");
        tEspinilleras = Recursos::cargarTexturaPowerUp("espinilleras");
        tGuantes = Recursos::cargarTexturaPowerUp("guantes");
        tBanda = Recursos::cargarTexturaPowerUp("banda");
        tAmarilla = Recursos::cargarTexturaPowerUp("amarilla");
        tRoja = Recursos::cargarTexturaPowerUp("roja");
        tCorazon = Recursos::cargarTexturaPowerUp("corazon");

        // Cargar texturas de enemigos
        for(int i = 1; i <= 5; ++i) {
            tEnemigos[i-1] = Recursos::cargarTexturaEnemigo(i);
        }

        // Cargar texturas de personajes para la pantalla de seleccion
        for(int i = 0; i < 6; ++i) {
            tPersonajes[i] = Recursos::cargarTexturaPersonaje(i);
        }

        // Cargar texturas de escenarios
        for(int i = 1; i <= 5; ++i) {
            tEscenarios[i-1] = Recursos::cargarEscenario(i);
        }

        tMenuFondo.loadFromFile(Recursos::rutaArchivo("assets/images/Portadajuego.png"));
        tMenuFondo.setSmooth(true);
        tInstruccionesFondo.loadFromFile(Recursos::rutaArchivo("assets/images/intruccionesdejuego.png"));
        tInstruccionesFondo.setSmooth(true);

        if (AUDIO_ACTIVO) {
        // Cargar sonidos
        bufLanzamiento = Recursos::cargarSonidoLanzamiento();
        bufSilbato = Recursos::cargarSonidoSilbato();
        bufEnemigoDerrota = Recursos::cargarSonidoEnemigoDerrota();
        bufDerrota = Recursos::cargarSonidoDerrota();
        bufGolpeJugador = Recursos::cargarSonidoGolpeJugador();
        bufRondaPerdida = Recursos::cargarSonidoRondaPerdida();
        bufVictoria = Recursos::cargarSonidoVictoria();

            sonidoLanzamiento = crearSonido(bufLanzamiento, 30.f);
            sonidoSilbato = crearSonido(bufSilbato, 30.f);
            sonidoEnemigoDerrota = crearSonido(bufEnemigoDerrota, 50.f);
            sonidoDerrota = crearSonido(bufRondaPerdida, 58.f);
            sonidoVictoria = crearSonido(bufVictoria, 50.f);
            sonidoHover = crearSonido(bufLanzamiento, 12.f);
            sonidoGritoGol = crearSonido(bufVictoria, 72.f, 1.08f);

            float tonosJugador[6] = {0.82f, 0.94f, 1.06f, 1.18f, 1.30f, 1.44f};
            float tonosGolpeJugador[6] = {0.88f, 0.95f, 1.02f, 1.09f, 1.16f, 1.23f};
            for (int i = 0; i < 6; ++i) {
                sonidosJugador[i] = crearSonido(bufLanzamiento, 32.f, tonosJugador[i]);
                sonidosGolpeJugador[i] = crearSonido(bufGolpeJugador, 54.f, tonosGolpeJugador[i]);
            }

            float tonosMuerte[5] = {0.82f, 0.95f, 1.08f, 1.22f, 1.38f};
            for (int i = 0; i < 5; ++i) {
                sonidosGolpeJefe[i] = crearSonido(bufLanzamiento, 28.f, tonosMuerte[i] + 0.18f);
                sonidosMuerteJefe[i] = crearSonido(bufEnemigoDerrota, 55.f, tonosMuerte[i]);
        }

        // Cargar música
        musicaFondo = Recursos::cargarMusicaFondo();
        musicaMenu = Recursos::cargarMusicaIntro();
        }
    }

    // METODO AJUSTADO A TU PIZARRON
    void Iniciar() {
        while (ventana.isOpen()) {
            float dt = relojFrame.restart().asSeconds();
            tiempoAnimacion += dt;
            if (estadoActual == GameState::JUGANDO) tiempoTotalJuego += dt;
            if (estadoActual == GameState::PANTALLA_NIVEL) tiempoTransicionNivel += dt;

            actualizarMusica();
            procesarEventos();
            actualizar(dt);
            renderizar();
        }
        
        if (musicaFondo) {
            musicaFondo->stop();
        }
        if (musicaMenu) {
            musicaMenu->stop();
        }
    }

    ~Juego() { 
        delete jefe;
        delete sonidoLanzamiento;
        delete sonidoSilbato;
        delete sonidoEnemigoDerrota;
        delete sonidoDerrota;
        delete sonidoVictoria;
        delete sonidoHover;
        delete sonidoGritoGol;
        for (int i = 0; i < 6; ++i) {
            delete sonidosJugador[i];
            delete sonidosGolpeJugador[i];
        }
        for (int i = 0; i < 5; ++i) {
            delete sonidosGolpeJefe[i];
            delete sonidosMuerteJefe[i];
        }
        if (musicaFondo) {
            musicaFondo->stop();
            delete musicaFondo;
        }
        if (musicaMenu) {
            musicaMenu->stop();
            delete musicaMenu;
        }
    }

private:
    void procesarEventos() {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) ventana.close();

            if (evento.type == sf::Event::MouseMoved) {
                sf::Vector2i pixel(evento.mouseMove.x, evento.mouseMove.y);
                actualizarHover(ventana.mapPixelToCoords(pixel));
            }

            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i pixel(evento.mouseButton.x, evento.mouseButton.y);
                sf::Vector2f mouse = ventana.mapPixelToCoords(pixel);
                if (estadoActual == GameState::MENU) {
                    int opcion = opcionMenuEn(mouse);
                    if (opcion >= 0) ejecutarOpcionMenu(opcion);
                } else if (estadoActual == GameState::PERSONAJES) {
                    int idx = personajeEn(mouse);
                    if (idx >= 0) {
                        iniciarPartidaConPersonaje(idx);
                    }
                } else if (estadoActual == GameState::GALERIA || estadoActual == GameState::INSTRUCCIONES || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
                    estadoActual = GameState::MENU;
                } else if (estadoActual == GameState::PANTALLA_NIVEL) {
                    iniciarNivel(nivel);
                    estadoActual = GameState::JUGANDO;
                }
            }

            if (evento.type == sf::Event::KeyPressed) {
                if (estadoActual == GameState::MENU) {
                    if (evento.key.code == sf::Keyboard::Num1) ejecutarOpcionMenu(0);
                    if (evento.key.code == sf::Keyboard::Num2) ejecutarOpcionMenu(1);
                    if (evento.key.code == sf::Keyboard::Num3) ejecutarOpcionMenu(2);
                    if (evento.key.code == sf::Keyboard::Num4) ejecutarOpcionMenu(3);
                    if (evento.key.code == sf::Keyboard::P) ejecutarOpcionMenu(0);
                    if (evento.key.code == sf::Keyboard::E) ejecutarOpcionMenu(1);
                    if (evento.key.code == sf::Keyboard::I) ejecutarOpcionMenu(2);
                }
                else if (estadoActual == GameState::PERSONAJES) {
                    if (evento.key.code >= sf::Keyboard::Num1 && evento.key.code <= sf::Keyboard::Num6) {
                        iniciarPartidaConPersonaje(evento.key.code - sf::Keyboard::Num1);
                    }
                    if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) estadoActual = GameState::MENU;
                }
                else if (estadoActual == GameState::GALERIA || estadoActual == GameState::INSTRUCCIONES || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
                    if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) estadoActual = GameState::MENU;
                }
                else if (estadoActual == GameState::PANTALLA_NIVEL) {
                    if (evento.key.code == sf::Keyboard::Space || evento.key.code == sf::Keyboard::Enter) {
                        iniciarNivel(nivel);
                        estadoActual = GameState::JUGANDO;
                    }
                }
                else if (estadoActual == GameState::JUGANDO) {
                    if (evento.key.code == sf::Keyboard::Up || evento.key.code == sf::Keyboard::W) jugador.saltar();
                    if (evento.key.code == sf::Keyboard::Space || evento.key.code == sf::Keyboard::Enter) {
                        intentarLanzarBalon();
                    }
                }
            }
        }
    }

    void actualizar(float dt) {
        if (estadoActual != GameState::JUGANDO) return;

        if (tiempoDisparoJugador > 0.f) tiempoDisparoJugador -= dt;
        if (tiempoFlashJefe > 0.f) tiempoFlashJefe -= dt;
        if (tiempoSacudida > 0.f) tiempoSacudida -= dt;
        if (tiempoGolCambio > 0.f) tiempoGolCambio -= dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) jugador.mover(-1.f, dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) jugador.mover(1.f, dt);

        jugador.actualizarFisica(dt);

        if (jefe) {
            jefe->actualizar(dt);
            jefe->atacar(dt, ataques, texturaProyectilEnemigo(), jugador.x);
        }

        if (poderActivo != PowerUpType::NINGUNO) {
            tiempoPoder -= dt;
            if (tiempoPoder <= 0.f) {
                poderActivo = PowerUpType::NINGUNO;
                jugador.desactivarPowerUps();
            }
        }

        if (poderActivo == PowerUpType::NINGUNO && powerups.empty()) {
            tiempoSpawnPowerUp += dt;
            if (tiempoSpawnPowerUp >= 8.5f) {
                tiempoSpawnPowerUp = 0.f;
                generarPowerUp();
            }
        }

        for (auto& b : balones) b.actualizar(dt);
        for (auto& a : ataques) a.actualizar(dt);
        for (auto& p : powerups) p.actualizar(dt);

        verificarColisiones();
    }

    void verificarColisiones() {
        for (auto it = balones.begin(); it != balones.end();) {
            if (jefe && it->sprite.getGlobalBounds().intersects(jefe->sprite.getGlobalBounds())) {
                it = balones.erase(it);
                jefe->vida--;
                reproducirSonido(sonidosGolpeJefe[std::max(0, std::min(4, nivel - 1))]);
                tiempoFlashJefe = 0.18f;
                tiempoSacudida = 0.10f;
                if (jefe->vida <= 0) {
                    puntos += 20;
                    ultimoJefeDerrotado = std::max(0, std::min(4, nivel - 1));
                    reproducirSonido(sonidosMuerteJefe[ultimoJefeDerrotado]);
                    if (nivel < 5) {
                        nivel++;
                        tiempoTransicionNivel = 0.f;
                        reproducirSonido(sonidoGritoGol);
                        estadoActual = GameState::PANTALLA_NIVEL;
                        return;
                    } else {
                        estadoActual = GameState::VICTORIA;
                        reproducirSonido(sonidoVictoria);
                        return;
                    }
                }
            } else if (it->sprite.getPosition().y < -20.f) {
                it = balones.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = ataques.begin(); it != ataques.end();) {
            if (it->sprite.getGlobalBounds().intersects(jugador.sprite.getGlobalBounds())) {
                it = ataques.erase(it);
                if (!jugador.invencible && !jugador.estaDerribado()) {
                    jugador.vidas--;
                    jugador.derribar(jugador.x - (jefe ? jefe->x : jugador.x));
                    reproducirSonido(sonidosGolpeJugador[personajeSeleccionadoIdx]);
                    if (jugador.vidas <= 0) {
                        estadoActual = GameState::GAMEOVER;
                        reproducirSonido(sonidoDerrota);
                        return;
                    }
                }
            } else if (it->sprite.getPosition().y > 620.f) {
                it = ataques.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = powerups.begin(); it != powerups.end();) {
            if (it->expiro()) {
                it = powerups.erase(it);
                tiempoSpawnPowerUp = 0.f;
            } else if (it->sprite.getGlobalBounds().intersects(jugador.sprite.getGlobalBounds())) {
                if (it->tipo == PowerUpType::VIDA_EXTRA) {
                    if (jugador.vidas < 3) {
                        jugador.vidas++;
                        reproducirSonido(sonidoSilbato);
                    }
                    it = powerups.erase(it);
                    tiempoSpawnPowerUp = 0.f;
                    continue;
                }

                poderActivo = it->tipo;
                tiempoSpawnPowerUp = 0.f;
                if(poderActivo == PowerUpType::TARJETA_ROJA) {
                    estadoActual = GameState::GAMEOVER;
                    reproducirSonido(sonidoDerrota);
                    return;
                }
                
                if (poderActivo == PowerUpType::TAQUETES) tiempoPoder = 5.f;
                else if (poderActivo == PowerUpType::ESPINILLERAS) tiempoPoder = 5.f;
                else if (poderActivo == PowerUpType::GUANTES) tiempoPoder = 10.f;
                else if (poderActivo == PowerUpType::BANDA_CAPITAN) tiempoPoder = 15.f;
                else if (poderActivo == PowerUpType::TARJETA_AMARILLA) tiempoPoder = 5.f;

                jugador.aplicarPowerUp(poderActivo);
                it = powerups.erase(it);
            } else if (it->sprite.getPosition().y > 620.f) {
                it = powerups.erase(it);
                tiempoSpawnPowerUp = 0.f;
            } else {
                ++it;
            }
        }
    }

    void renderizar() {
        ventana.clear(sf::Color(34, 139, 34)); 

        if (estadoActual == GameState::MENU) {
            ventana.clear(sf::Color(2, 9, 24));

            if (tMenuFondo.getSize().x > 0 && tMenuFondo.getSize().y > 0) {
                sf::Sprite portada(tMenuFondo);
                float escala = std::min(
                    800.f / portada.getLocalBounds().width,
                    600.f / portada.getLocalBounds().height
                );
                portada.setScale(escala, escala);
                portada.setPosition(
                    (800.f - portada.getLocalBounds().width * escala) * 0.5f,
                    (600.f - portada.getLocalBounds().height * escala) * 0.5f
                );
                ventana.draw(portada);
            } else {
                dibujarFondoMenu();
            }

            if (opcionMenuHover >= 0) {
                sf::FloatRect rect = rectBotonMenu(opcionMenuHover);
                float pulso = (std::sin(tiempoAnimacion * 8.f) + 1.f) * 0.5f;

                sf::RectangleShape brillo(sf::Vector2f(rect.width, rect.height));
                brillo.setPosition(rect.left, rect.top);
                brillo.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(22 + pulso * 22)));
                ventana.draw(brillo);

                sf::ConvexShape indicador;
                indicador.setPointCount(3);
                float x = rect.left - 16.f + pulso * 5.f;
                float y = rect.top + rect.height * 0.5f;
                if (opcionMenuHover == 0) {
                    x = rect.left + rect.width * 0.5f - 16.f;
                    y = rect.top + rect.height + 15.f + pulso * 5.f;
                    indicador.setPoint(0, sf::Vector2f(x, y));
                    indicador.setPoint(1, sf::Vector2f(x + 32.f, y));
                    indicador.setPoint(2, sf::Vector2f(x + 16.f, y + 22.f));
                } else {
                    indicador.setPoint(0, sf::Vector2f(x, y - 15.f));
                    indicador.setPoint(1, sf::Vector2f(x, y + 15.f));
                    indicador.setPoint(2, sf::Vector2f(x + 24.f, y));
                }
                indicador.setFillColor(sf::Color(245, 210, 65, 235));
                ventana.draw(indicador);
            }
        } 
        else if (estadoActual == GameState::PERSONAJES) {
            dibujarPantallaPersonajes();
        }
        else if (estadoActual == GameState::INSTRUCCIONES) {
            dibujarPantallaInstrucciones();
        }
        else if (estadoActual == GameState::GALERIA) {
            dibujarFondoMenu();
            sf::RectangleShape panel(sf::Vector2f(700.f, 500.f));
            panel.setPosition(50.f, 55.f);
            panel.setFillColor(sf::Color(0, 0, 0, 165));
            panel.setOutlineColor(sf::Color(80, 210, 255));
            panel.setOutlineThickness(2.f);
            ventana.draw(panel);

            dibujarTextoSombra("GALERIA DE RIVALES", 185, 82, 38, sf::Color::White);
            std::string ataquesTxt[5] = {"Silbatos", "Silbatos", "Dinero", "Flechas", "Dinero"};

            for(int i = 0; i < 5; ++i) {
                float y = 155.f + i * 66.f;
                sf::RectangleShape fila(sf::Vector2f(590.f, 48.f));
                fila.setPosition(105.f, y);
                fila.setFillColor(sf::Color(12, 24, 24, 220));
                fila.setOutlineColor(sf::Color(235, 235, 235, 60));
                fila.setOutlineThickness(1.f);
                ventana.draw(fila);

                sf::Sprite rival(tEnemigos[i]);
                sf::Vector2u size = tEnemigos[i].getSize();
                if (size.x > 0 && size.y > 0) {
                    int frameW = static_cast<int>(size.x / 3);
                    int frameH = static_cast<int>(size.y / 2);
                    rival.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
                    rival.setOrigin(frameW / 2.f, frameH * 0.85f);
                    float escala = std::min(44.f / frameW, 54.f / frameH);
                    rival.setScale(escala, escala);
                    rival.setPosition(136.f, y + 45.f);
                    ventana.draw(rival);
                }

                ui.dibujarTexto(ventana, "NIVEL " + std::to_string(i + 1), 178, y + 7, 15, sf::Color(245, 210, 65));
                ui.dibujarTexto(ventana, nombresEnemigos[i], 285, y + 7, 17, sf::Color::White);
                ui.dibujarTexto(ventana, "Ataque: " + ataquesTxt[i], 500, y + 9, 15, sf::Color(110, 245, 150));
            }

            ui.dibujarTexto(ventana, "ENTER / ESC para regresar", 285, 500, 18, sf::Color(245, 210, 65));
        }
        else if (estadoActual == GameState::JUGANDO) {
            // Dibujar escenario de fondo
            if (nivel >= 1 && nivel <= 5 && tEscenarios[nivel - 1].getSize().x > 0 && tEscenarios[nivel - 1].getSize().y > 0) {
                escenarioActual.setTexture(tEscenarios[nivel - 1]);
                escenarioActual.setScale(
                    800.f / escenarioActual.getLocalBounds().width,
                    600.f / escenarioActual.getLocalBounds().height
                );
                escenarioActual.setPosition(0, 0);
                ventana.draw(escenarioActual);
            } else {
                sf::RectangleShape campo(sf::Vector2f(800.f, 600.f));
                campo.setFillColor(sf::Color(26, 120, 54));
                ventana.draw(campo);

                sf::RectangleShape mediaCancha(sf::Vector2f(800.f, 4.f));
                mediaCancha.setPosition(0.f, 300.f);
                mediaCancha.setFillColor(sf::Color(225, 245, 225, 110));
                ventana.draw(mediaCancha);

                sf::CircleShape circulo(74.f);
                circulo.setOrigin(74.f, 74.f);
                circulo.setPosition(400.f, 300.f);
                circulo.setFillColor(sf::Color::Transparent);
                circulo.setOutlineColor(sf::Color(225, 245, 225, 120));
                circulo.setOutlineThickness(4.f);
                ventana.draw(circulo);
            }
            
            sf::CircleShape sombraJugador(36.f);
            sombraJugador.setOrigin(36.f, 10.f);
            sombraJugador.setScale(1.35f, 0.28f);
            sombraJugador.setPosition(jugador.x, 548.f);
            sombraJugador.setFillColor(sf::Color(0, 0, 0, 115));
            ventana.draw(sombraJugador);

            ventana.draw(jugador.sprite);
            if (jefe) {
                sf::CircleShape sombraJefe(34.f);
                sombraJefe.setOrigin(34.f, 10.f);
                sombraJefe.setScale(1.35f, 0.24f);
                sombraJefe.setPosition(jefe->sprite.getPosition().x, jefe->sprite.getPosition().y + 48.f);
                sombraJefe.setFillColor(sf::Color(0, 0, 0, 105));
                ventana.draw(sombraJefe);

                sf::Sprite jefeAnimado = jefe->sprite;
                if (tiempoSacudida > 0.f) {
                    float sacudirX = std::sin(tiempoAnimacion * 90.f) * 7.f;
                    float sacudirY = std::cos(tiempoAnimacion * 80.f) * 4.f;
                    jefeAnimado.move(sacudirX, sacudirY);
                    jefeAnimado.setColor(sf::Color(255, 145, 145));
                } else {
                    jefeAnimado.setColor(sf::Color::White);
                }
                ventana.draw(jefeAnimado);

                float hpRatio = std::max(0.f, std::min(1.f, jefe->vida / static_cast<float>(Enemigo::VIDA_MAXIMA)));
                sf::RectangleShape hpBack(sf::Vector2f(86.f, 8.f));
                hpBack.setOrigin(43.f, 4.f);
                hpBack.setPosition(jefe->sprite.getPosition().x, jefe->sprite.getPosition().y - 44.f);
                hpBack.setFillColor(sf::Color(0, 0, 0, 175));
                ventana.draw(hpBack);

                sf::RectangleShape hpFill(sf::Vector2f(82.f * hpRatio, 5.f));
                hpFill.setOrigin(41.f, 2.5f);
                hpFill.setPosition(jefe->sprite.getPosition().x, jefe->sprite.getPosition().y - 44.f);
                hpFill.setFillColor(hpRatio < 0.35f ? sf::Color(255, 80, 70) : sf::Color(110, 245, 150));
                ventana.draw(hpFill);
            }
            for (auto& b : balones) ventana.draw(b.sprite);
            for (auto& a : ataques) ventana.draw(a.sprite);
            for (auto& p : powerups) dibujarPowerUpConFeedback(p);
            dibujarFlashGolpeJefe();

            ui.dibujarHUD(ventana, puntos, nivel, jugador.vidas, (jefe ? jefe->vida : 0), poderActivo, tiempoPoder);
            dibujarGolCambioEnemigo();
        }
        else if (estadoActual == GameState::PANTALLA_NIVEL) {
            dibujarTransicionNivel();
        }
        else if (estadoActual == GameState::VICTORIA) {
            dibujarPantallaVictoria();
        }
        else if (estadoActual == GameState::GAMEOVER) {
            dibujarPantallaGameOver();
        }

        ventana.display();
    }
};
