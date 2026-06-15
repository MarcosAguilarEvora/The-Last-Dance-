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

    sf::Texture tBalon, tSilbato, tDinero, tFlecha, tTaquetes, tEspinilleras, tGuantes, tBanda, tAmarilla, tRoja;
    sf::Texture tEnemigos[5];
    sf::Texture tPersonajes[6];
    sf::Texture tEscenarios[5];
    sf::Texture tMenuFondo;

    // Audio
    sf::Music* musicaFondo;
    sf::Music* musicaMenu;
    bool musicaJuegoActiva;
    sf::Sound *sonidoLanzamiento, *sonidoSilbato, *sonidoEnemigoDerrota, *sonidoDerrota, *sonidoVictoria, *sonidoHover;
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
            default: return tTaquetes;
        }
    }

    sf::Texture& texturaProyectilEnemigo() {
        if (nivel == 3 || nivel == 5) return tDinero;
        if (nivel == 4) return tFlecha;
        return tSilbato;
    }

    void generarPowerUp() {
        PowerUpType randomTipo = static_cast<PowerUpType>((std::rand() % 6) + 1);
        float posicionesY[4] = {520.f, 455.f, 405.f, 365.f};
        float x = static_cast<float>(std::rand() % 680 + 60);
        float y = posicionesY[std::rand() % 4];

        powerups.clear();
        powerups.push_back(PowerUp(x, y, randomTipo, texturaParaPowerUp(randomTipo)));
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
        return sf::FloatRect(500.f, 226.f + opcion * 68.f, 265.f, 56.f);
    }

    sf::FloatRect rectPersonaje(int idx) {
        int col = idx % 3;
        int row = idx / 3;
        return sf::FloatRect(75.f + col * 225.f, 170.f + row * 145.f, 190.f, 112.f);
    }

    int opcionMenuEn(sf::Vector2f punto) {
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
        if (idx == 2) return 3;
        if (idx == 3) return 3;
        if (idx == 5) return 8;
        return idx == 4 ? 5 : 4;
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
        panel.setFillColor(sf::Color(0, 0, 0, 175));
        panel.setOutlineColor(sf::Color(245, 210, 65));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        dibujarTextoSombra("ELIGE TU PERSONAJE", 185.f, 80.f, 38, sf::Color::White);
        ui.dibujarTexto(ventana, "Haz click en un futbolista para iniciar el partido.", 195.f, 125.f, 16, sf::Color(210, 240, 220));

        for(int i = 0; i < 6; ++i) {
            sf::FloatRect rect = rectPersonaje(i);
            bool hover = personajeHover == i;

            sf::RectangleShape card(sf::Vector2f(rect.width, rect.height));
            card.setPosition(rect.left, rect.top);
            card.setFillColor(hover ? sf::Color(24, 66, 52, 240) : sf::Color(12, 24, 24, 225));
            card.setOutlineColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
            card.setOutlineThickness(hover ? 3.f : 2.f);
            ventana.draw(card);

            sf::Sprite preview(tPersonajes[i]);
            int columnas = columnasPersonaje(i);
            if (columnas > 0) {
                int frame = static_cast<int>(tiempoAnimacion * 6.f + i) % columnas;
                preview.setTextureRect(sf::IntRect(frame * (static_cast<int>(tPersonajes[i].getSize().x) / columnas), 0, static_cast<int>(tPersonajes[i].getSize().x) / columnas, static_cast<int>(tPersonajes[i].getSize().y)));
            }
            sf::FloatRect bounds = preview.getLocalBounds();
            if (bounds.width > 0.f && bounds.height > 0.f) {
                preview.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                float escala = std::min(88.f / bounds.width, 98.f / bounds.height);
                preview.setScale(escala, escala);
                preview.setPosition(rect.left + 58.f, rect.top + 58.f);

                sf::CircleShape resplandor(38.f);
                resplandor.setOrigin(38.f, 38.f);
                resplandor.setScale(1.05f, 0.95f);
                resplandor.setPosition(rect.left + 58.f, rect.top + 58.f);
                resplandor.setFillColor(sf::Color(170, 235, 205, 32));
                ventana.draw(resplandor);

                sf::CircleShape sombra(29.f);
                sombra.setOrigin(29.f, 9.f);
                sombra.setScale(1.4f, 0.27f);
                sombra.setPosition(rect.left + 58.f, rect.top + 96.f);
                sombra.setFillColor(sf::Color(0, 0, 0, 120));
                ventana.draw(sombra);

                ventana.draw(preview);
            }

            sf::CircleShape numero(13.f);
            numero.setPosition(rect.left + 12.f, rect.top + 11.f);
            numero.setFillColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
            numero.setOutlineColor(sf::Color::Black);
            numero.setOutlineThickness(2.f);
            ventana.draw(numero);

            ui.dibujarTexto(ventana, std::to_string(i + 1), rect.left + 19.f, rect.top + 13.f, 13, sf::Color::Black);
            ui.dibujarTexto(ventana, nombresPersonajes[i], rect.left + 92.f, rect.top + 28.f, 13, sf::Color::White);
            ui.dibujarTexto(ventana, hover ? "Jugar ahora" : "Elegir", rect.left + 92.f, rect.top + 58.f, 12, hover ? sf::Color(245, 210, 65) : sf::Color(210, 240, 220));
        }

        ui.dibujarTexto(ventana, "ENTER / ESC para regresar", 285.f, 515.f, 18, sf::Color(245, 210, 65));
    }

    void dibujarPantallaInstrucciones() {
        dibujarFondoMenu();

        sf::RectangleShape panel(sf::Vector2f(700.f, 500.f));
        panel.setPosition(50.f, 55.f);
        panel.setFillColor(sf::Color(0, 0, 0, 175));
        panel.setOutlineColor(sf::Color(110, 245, 150));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        dibujarTextoSombra("INSTRUCCIONES", 245.f, 85.f, 38, sf::Color::White);

        ui.dibujarTexto(ventana, "En el menu:", 115.f, 155.f, 20, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "1/P: ver personajes  |  2/E: ver enemigos  |  3/I: instrucciones", 115.f, 190.f, 16, sf::Color::White);
        ui.dibujarTexto(ventana, "En personajes, haz click o presiona 1-6 para iniciar partido.", 115.f, 222.f, 16, sf::Color::White);

        ui.dibujarTexto(ventana, "Durante el partido:", 115.f, 285.f, 20, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Flechas: moverte  |  W/Arriba: saltar", 115.f, 320.f, 16, sf::Color::White);
        ui.dibujarTexto(ventana, "Espacio: lanzar balon", 115.f, 352.f, 16, sf::Color::White);
        ui.dibujarTexto(ventana, "Derrota a los 5 rivales y llega a 100 puntos.", 115.f, 384.f, 16, sf::Color(110, 245, 150));

        ui.dibujarTexto(ventana, "ENTER / ESC para regresar", 285.f, 500.f, 18, sf::Color(245, 210, 65));
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
        dibujarFondoMenu();

        sf::RectangleShape capa(sf::Vector2f(800.f, 600.f));
        capa.setFillColor(sf::Color(0, 0, 0, 95));
        ventana.draw(capa);

        dibujarLucesEstadio(78.f);

        float entrada = std::min(1.f, tiempoTransicionNivel / 0.55f);
        float panelY = 65.f + (1.f - entrada) * 36.f;
        float pulso = (std::sin(tiempoAnimacion * 5.f) + 1.f) * 0.5f;
        int nivelAnterior = std::max(1, nivel - 1);

        sf::RectangleShape panel(sf::Vector2f(690.f, 475.f));
        panel.setPosition(55.f, panelY);
        panel.setFillColor(sf::Color(6, 20, 18, 235));
        panel.setOutlineColor(sf::Color(245, 210, 65));
        panel.setOutlineThickness(3.f);
        ventana.draw(panel);

        sf::RectangleShape banda(sf::Vector2f(690.f, 64.f));
        banda.setPosition(55.f, panelY);
        banda.setFillColor(sf::Color(18, 115, 60, 235));
        ventana.draw(banda);

        float brilloX = 55.f + std::fmod(tiempoAnimacion * 260.f, 690.f);
        sf::RectangleShape brillo(sf::Vector2f(95.f, 475.f));
        brillo.setPosition(brilloX, panelY);
        brillo.setFillColor(sf::Color(255, 255, 255, 22));
        ventana.draw(brillo);

        dibujarTextoSombra("TRANSICION DE NIVEL", 175.f, panelY + 12.f, 34, sf::Color::White);

        sf::RectangleShape tarjetaIzq(sf::Vector2f(250.f, 170.f));
        tarjetaIzq.setPosition(95.f, panelY + 105.f);
        tarjetaIzq.setFillColor(sf::Color(10, 30, 28, 245));
        tarjetaIzq.setOutlineColor(sf::Color(245, 210, 65));
        tarjetaIzq.setOutlineThickness(2.f);
        ventana.draw(tarjetaIzq);

        sf::RectangleShape tarjetaDer(sf::Vector2f(250.f, 170.f));
        tarjetaDer.setPosition(455.f, panelY + 105.f);
        tarjetaDer.setFillColor(sf::Color(10, 30, 28, 245));
        tarjetaDer.setOutlineColor(sf::Color(80, 210, 255));
        tarjetaDer.setOutlineThickness(2.f + pulso);
        ventana.draw(tarjetaDer);

        sf::Sprite estadioAnterior(tEscenarios[nivelAnterior - 1]);
        estadioAnterior.setScale(250.f / estadioAnterior.getLocalBounds().width, 120.f / estadioAnterior.getLocalBounds().height);
        estadioAnterior.setPosition(95.f, panelY + 105.f);
        estadioAnterior.setColor(sf::Color(255, 255, 255, 150));
        ventana.draw(estadioAnterior);

        sf::Sprite estadioSiguiente(tEscenarios[nivel - 1]);
        estadioSiguiente.setScale(250.f / estadioSiguiente.getLocalBounds().width, 120.f / estadioSiguiente.getLocalBounds().height);
        estadioSiguiente.setPosition(455.f, panelY + 105.f);
        estadioSiguiente.setColor(sf::Color(255, 255, 255, 230));
        ventana.draw(estadioSiguiente);

        sf::RectangleShape sombraMiniIzq(sf::Vector2f(250.f, 50.f));
        sombraMiniIzq.setPosition(95.f, panelY + 225.f);
        sombraMiniIzq.setFillColor(sf::Color(0, 0, 0, 165));
        ventana.draw(sombraMiniIzq);

        sf::RectangleShape sombraMiniDer(sf::Vector2f(250.f, 50.f));
        sombraMiniDer.setPosition(455.f, panelY + 225.f);
        sombraMiniDer.setFillColor(sf::Color(0, 0, 0, 165));
        ventana.draw(sombraMiniDer);

        ui.dibujarTexto(ventana, "NIVEL " + std::to_string(nivelAnterior), 170.f, panelY + 235.f, 24, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "NIVEL " + std::to_string(nivel), 530.f, panelY + 235.f, 24, sf::Color(80, 210, 255));

        sf::CircleShape circuloFlecha(42.f);
        circuloFlecha.setOrigin(42.f, 42.f);
        circuloFlecha.setPosition(400.f, panelY + 190.f);
        circuloFlecha.setFillColor(sf::Color(245, 210, 65, 230));
        circuloFlecha.setOutlineColor(sf::Color::Black);
        circuloFlecha.setOutlineThickness(3.f);
        ventana.draw(circuloFlecha);

        sf::ConvexShape flecha(3);
        flecha.setPoint(0, sf::Vector2f(382.f, panelY + 168.f));
        flecha.setPoint(1, sf::Vector2f(382.f, panelY + 212.f));
        flecha.setPoint(2, sf::Vector2f(424.f, panelY + 190.f));
        flecha.setFillColor(sf::Color(8, 22, 18));
        ventana.draw(flecha);

        ui.dibujarTexto(ventana, "Rival derrotado: " + nombresEnemigos[ultimoJefeDerrotado], 95.f, panelY + 308.f, 20, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Siguiente rival: " + nombresEnemigos[nivel - 1], 95.f, panelY + 344.f, 20, sf::Color(80, 210, 255));
        ui.dibujarTexto(ventana, "Puntos: " + std::to_string(puntos) + "   Vidas: " + std::to_string(jugador.vidas), 95.f, panelY + 380.f, 20, sf::Color::White);

        sf::RectangleShape barra(sf::Vector2f(500.f, 12.f));
        barra.setPosition(150.f, panelY + 420.f);
        barra.setFillColor(sf::Color(35, 35, 35, 230));
        ventana.draw(barra);

        sf::RectangleShape progreso(sf::Vector2f(std::min(500.f, tiempoTransicionNivel * 160.f), 12.f));
        progreso.setPosition(150.f, panelY + 420.f);
        progreso.setFillColor(sf::Color(245, 210, 65));
        ventana.draw(progreso);

        sf::RectangleShape botonContinuar(sf::Vector2f(330.f, 44.f));
        botonContinuar.setPosition(235.f, panelY + 445.f);
        botonContinuar.setFillColor(sf::Color(245, 197, 66));
        botonContinuar.setOutlineColor(sf::Color::Black);
        botonContinuar.setOutlineThickness(2.f);
        ventana.draw(botonContinuar);
        ui.dibujarTexto(ventana, "CLICK PARA CONTINUAR", 306.f, panelY + 457.f, 17, sf::Color::Black);
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
              sonidoLanzamiento(nullptr), sonidoSilbato(nullptr), sonidoEnemigoDerrota(nullptr), sonidoDerrota(nullptr), sonidoVictoria(nullptr), sonidoHover(nullptr) {
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
                    if (evento.key.code == sf::Keyboard::Num4) opcionMenuHover = 3;
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
            dibujarFondoMenu();
            dibujarEfectosPortada();

            sf::RectangleShape tituloBack(sf::Vector2f(430.f, 118.f));
            tituloBack.setPosition(38.f, 54.f);
            tituloBack.setFillColor(sf::Color(0, 0, 0, 115));
            tituloBack.setOutlineColor(sf::Color(80, 210, 255, 180));
            tituloBack.setOutlineThickness(2.f);
            ventana.draw(tituloBack);

            dibujarTextoSombra("THE LAST", 58.f, 65.f, 48, sf::Color(245, 245, 245));
            dibujarTextoSombra("DANCE", 58.f, 108.f, 58, sf::Color(245, 210, 65));
            ui.dibujarTexto(ventana, "COPA DE BARRIO", 62.f, 178.f, 17, sf::Color(110, 245, 150));

            sf::RectangleShape panelMenu(sf::Vector2f(305.f, 338.f));
            panelMenu.setPosition(477.f, 196.f);
            panelMenu.setFillColor(sf::Color(0, 0, 0, 120));
            panelMenu.setOutlineColor(sf::Color(255, 255, 255, 55));
            panelMenu.setOutlineThickness(2.f);
            ventana.draw(panelMenu);

            sf::RectangleShape barraPanel(sf::Vector2f(235.f, 5.f));
            barraPanel.setPosition(512.f, 211.f);
            barraPanel.setFillColor(sf::Color(245, 210, 65));
            ventana.draw(barraPanel);

            ui.dibujarTexto(ventana, "Futbol Mexicano Multiverse", 500.f, 178.f, 15, sf::Color(110, 245, 150));

            dibujarPersonajeMenu(0, 170.f, 455.f, 170.f, false);
            dibujarPersonajeMenu(1, 290.f, 462.f, 150.f, false);
            dibujarPersonajeMenu(4, 405.f, 455.f, 160.f, true);

            if (tBalon.getSize().x > 0 && tBalon.getSize().y > 0) {
                sf::Sprite balonMenu(tBalon);
                balonMenu.setOrigin(tBalon.getSize().x / 2.f, tBalon.getSize().y / 2.f);
                balonMenu.setScale(0.42f, 0.42f);
                balonMenu.setRotation(tiempoAnimacion * 130.f);
                balonMenu.setPosition(385.f + std::sin(tiempoAnimacion * 2.2f) * 55.f, 240.f + std::cos(tiempoAnimacion * 2.8f) * 22.f);
                ventana.draw(balonMenu);
            }

            dibujarBotonMenu(0, "PLAY", 245, sf::Color(245, 210, 65));
            dibujarBotonMenu(1, "RIVALES", 311, sf::Color(150, 75, 190));
            dibujarBotonMenu(2, "INSTRUCCIONES", 377, sf::Color(50, 175, 35));
            dibujarBotonMenu(3, "SALIR", 443, sf::Color(210, 55, 35));

            dibujarResumenPortada();
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
                    int frame = static_cast<int>(tiempoAnimacion * 5.f + i) % 3;
                    rival.setTextureRect(sf::IntRect(frame * frameW, 0, frameW, frameH));
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

                float hpRatio = std::max(0.f, std::min(1.f, jefe->vida / static_cast<float>(18 + nivel * 5)));
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
            for (auto& p : powerups) ventana.draw(p.sprite);
            dibujarFlashGolpeJefe();

            ui.dibujarHUD(ventana, puntos, nivel, jugador.vidas, (jefe ? jefe->vida : 0), poderActivo, tiempoPoder);
        }
        else if (estadoActual == GameState::PANTALLA_NIVEL) {
            dibujarTransicionNivel();
        }
        else if (estadoActual == GameState::VICTORIA) {
            ui.dibujarTexto(ventana, "¡ERES EL CAMPEON DE LA COPA!", 160, 150, 35, sf::Color::Yellow);
            ui.dibujarTexto(ventana, "Puntaje Final: " + std::to_string(puntos), 280, 240, 24, sf::Color::White);
            ui.dibujarTexto(ventana, "Tiempo de Juego: " + std::to_string((int)tiempoTotalJuego) + " segundos", 220, 290, 24, sf::Color::White);
            ui.dibujarTexto(ventana, "Presiona [ENTER] para volver al Menu", 200, 400, 22, sf::Color::Green);
        }
        else if (estadoActual == GameState::GAMEOVER) {
            ui.dibujarTexto(ventana, "GAME OVER (TARJETA ROJA O SIN VIDAS)", 120, 150, 32, sf::Color::Red);
            ui.dibujarTexto(ventana, "Puntaje Alcanzado: " + std::to_string(puntos), 260, 240, 24, sf::Color::White);
            ui.dibujarTexto(ventana, "Nivel Alcanzado: Nvl " + std::to_string(nivel), 260, 290, 24, sf::Color::White);
            ui.dibujarTexto(ventana, "Presiona [ENTER] para ir al Menu Principal", 180, 400, 22, sf::Color::Yellow);
        }

        ventana.display();
    }
};
