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

enum class GameState { MENU, GALERIA, JUGANDO, PANTALLA_NIVEL, VICTORIA, GAMEOVER };

class Juego {
private:
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
    sf::Texture tEscenarios[5];
    sf::Texture tMenuFondo;

    // Audio
    sf::Music* musicaFondo;
    sf::Sound sonidoLanzamiento, sonidoSilbato, sonidoEnemigoDerrota, sonidoDerrota, sonidoVictoria, sonidoHover;
    sf::Sound sonidosMuerteJefe[5];
    sf::SoundBuffer bufLanzamiento, bufSilbato, bufEnemigoDerrota, bufDerrota, bufVictoria;

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
        tiempoFlashJefe = 0.f;
        tiempoSacudida = 0.f;
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

        if (powerups.size() >= 4) {
            powerups.erase(powerups.begin());
        }

        powerups.push_back(PowerUp(x, y, randomTipo, texturaParaPowerUp(randomTipo)));
    }

    void dibujarFondoMenu() {
        if (tMenuFondo.getSize().x > 0 && tMenuFondo.getSize().y > 0) {
            menuFondo.setTexture(tMenuFondo);
            float scaleX = 800.f / static_cast<float>(tMenuFondo.getSize().x);
            float scaleY = 600.f / static_cast<float>(tMenuFondo.getSize().y);
            float scale = scaleX > scaleY ? scaleX : scaleY;
            menuFondo.setScale(scale, scale);
            menuFondo.setPosition(
                (800.f - tMenuFondo.getSize().x * scale) / 2.f,
                (600.f - tMenuFondo.getSize().y * scale) / 2.f
            );
            ventana.draw(menuFondo);
        } else {
            ventana.clear(sf::Color(20, 90, 45));
        }

        sf::RectangleShape capa(sf::Vector2f(800.f, 600.f));
        capa.setFillColor(sf::Color(0, 0, 0, 115));
        ventana.draw(capa);
    }

    void dibujarTextoSombra(const std::string& texto, float x, float y, int tam, sf::Color color) {
        ui.dibujarTexto(ventana, texto, x + 3.f, y + 3.f, tam, sf::Color(0, 0, 0, 190));
        ui.dibujarTexto(ventana, texto, x, y, tam, color);
    }

    sf::FloatRect rectBotonMenu(int opcion) {
        return sf::FloatRect(58.f, 396.f + opcion * 68.f, 380.f, 58.f);
    }

    sf::FloatRect rectPersonaje(int idx) {
        int col = idx % 2;
        int row = idx / 2;
        return sf::FloatRect(486.f + col * 142.f, 138.f + row * 54.f, 124.f, 40.f);
    }

    int opcionMenuEn(sf::Vector2f punto) {
        for (int i = 0; i < 2; ++i) {
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

    void reproducirHoverSiCambio(int nuevoHover, int& hoverActual) {
        if (nuevoHover != hoverActual) {
            hoverActual = nuevoHover;
            if (nuevoHover >= 0) sonidoHover.play();
        }
    }

    void actualizarHover(sf::Vector2f punto) {
        if (estadoActual == GameState::MENU) {
            int nuevoPersonajeHover = personajeEn(punto);
            int nuevoOpcionHover = nuevoPersonajeHover >= 0 ? -1 : opcionMenuEn(punto);
            reproducirHoverSiCambio(nuevoOpcionHover, opcionMenuHover);
            reproducirHoverSiCambio(nuevoPersonajeHover, personajeHover);
        } else {
            opcionMenuHover = -1;
            personajeHover = -1;
        }
    }

    void iniciarPartidaConPersonaje(int idx) {
        personajeSeleccionadoIdx = idx;
        jugador.textura = Recursos::cargarTexturaPersonaje(personajeSeleccionadoIdx);
        jugador.configurarSprite();

        nivel = 1;
        puntos = 0;
        jugador.vidas = 3;
        tiempoTotalJuego = 0.f;
        iniciarNivel(nivel);
        estadoActual = GameState::JUGANDO;
        opcionMenuHover = -1;
        personajeHover = -1;
    }

    void ejecutarOpcionMenu(int opcion) {
        if (opcion == 0) estadoActual = GameState::GALERIA;
        else if (opcion == 1) ventana.close();
        opcionMenuHover = -1;
        personajeHover = -1;
    }

    void dibujarBotonMenu(int opcion, const std::string& texto, float y, sf::Color color) {
        bool hover = opcionMenuHover == opcion;
        sf::FloatRect rect = rectBotonMenu(opcion);

        sf::RectangleShape sombra(sf::Vector2f(rect.width, rect.height));
        sombra.setPosition(rect.left + 6.f, rect.top + 7.f);
        sombra.setFillColor(sf::Color(0, 0, 0, 150));
        ventana.draw(sombra);

        sf::RectangleShape boton(sf::Vector2f(rect.width, rect.height));
        boton.setPosition(rect.left, rect.top);
        boton.setFillColor(hover ? sf::Color(22, 48, 39, 245) : sf::Color(12, 24, 22, 225));
        boton.setOutlineColor(hover ? sf::Color::White : color);
        boton.setOutlineThickness(hover ? 3.f : 2.f);
        ventana.draw(boton);

        sf::CircleShape balon(15.f);
        balon.setPosition(rect.left + 18.f, rect.top + 14.f);
        balon.setFillColor(color);
        balon.setOutlineColor(sf::Color::Black);
        balon.setOutlineThickness(2.f);
        ventana.draw(balon);

        sf::ConvexShape flecha(3);
        flecha.setPoint(0, sf::Vector2f(rect.left + rect.width - 32.f, rect.top + 20.f));
        flecha.setPoint(1, sf::Vector2f(rect.left + rect.width - 18.f, rect.top + 29.f));
        flecha.setPoint(2, sf::Vector2f(rect.left + rect.width - 32.f, rect.top + 38.f));
        flecha.setFillColor(hover ? color : sf::Color(190, 190, 190));
        ventana.draw(flecha);

        ui.dibujarTexto(ventana, texto, rect.left + 66.f, y + 17.f, 20, hover ? sf::Color(245, 245, 245) : sf::Color::White);
    }

    void dibujarPanelMenu() {
        sf::RectangleShape panel(sf::Vector2f(430.f, 520.f));
        panel.setPosition(32.f, 40.f);
        panel.setFillColor(sf::Color(0, 0, 0, 150));
        panel.setOutlineColor(sf::Color(235, 235, 235, 80));
        panel.setOutlineThickness(2.f);
        ventana.draw(panel);

        sf::RectangleShape linea(sf::Vector2f(180.f, 5.f));
        linea.setPosition(56.f, 92.f);
        linea.setFillColor(sf::Color(50, 210, 110));
        ventana.draw(linea);
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
        capa.setFillColor(sf::Color(0, 0, 0, 80));
        ventana.draw(capa);

        dibujarLucesEstadio(92.f);

        float entrada = tiempoTransicionNivel;
        float panelX = entrada < 0.45f ? -620.f + (entrada / 0.45f) * 740.f : 120.f;

        sf::RectangleShape panel(sf::Vector2f(560.f, 340.f));
        panel.setPosition(panelX, 125.f);
        panel.setFillColor(sf::Color(8, 22, 18, 230));
        panel.setOutlineColor(sf::Color(245, 210, 65));
        panel.setOutlineThickness(3.f);
        ventana.draw(panel);

        sf::RectangleShape banda(sf::Vector2f(560.f, 58.f));
        banda.setPosition(panelX, 125.f);
        banda.setFillColor(sf::Color(18, 120, 65, 230));
        ventana.draw(banda);

        float brilloX = 120.f + std::fmod(tiempoAnimacion * 240.f, 560.f);
        sf::RectangleShape brillo(sf::Vector2f(80.f, 340.f));
        brillo.setPosition(brilloX, 125.f);
        brillo.setFillColor(sf::Color(255, 255, 255, 25));
        ventana.draw(brillo);

        dibujarTextoSombra("NIVEL COMPLETADO", panelX + 118.f, 136.f, 32, sf::Color::White);
        ui.dibujarTexto(ventana, "Jefe derrotado: " + nombresEnemigos[ultimoJefeDerrotado], panelX + 64.f, 218.f, 20, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "Puntos acumulados: " + std::to_string(puntos), panelX + 64.f, 263.f, 20, sf::Color::White);
        ui.dibujarTexto(ventana, "Vidas restantes: " + std::to_string(jugador.vidas), panelX + 64.f, 305.f, 20, sf::Color(110, 245, 150));
        if(nivel <= 5) ui.dibujarTexto(ventana, "Siguiente rival: " + nombresEnemigos[nivel-1], panelX + 64.f, 347.f, 20, sf::Color(80, 210, 255));

        sf::RectangleShape barra(sf::Vector2f(410.f, 12.f));
        barra.setPosition(panelX + 75.f, 392.f);
        barra.setFillColor(sf::Color(40, 40, 40, 230));
        ventana.draw(barra);

        sf::RectangleShape progreso(sf::Vector2f(std::min(410.f, tiempoTransicionNivel * 135.f), 12.f));
        progreso.setPosition(panelX + 75.f, 392.f);
        progreso.setFillColor(sf::Color(245, 210, 65));
        ventana.draw(progreso);

        sf::RectangleShape botonContinuar(sf::Vector2f(330.f, 52.f));
        botonContinuar.setPosition(panelX + 115.f, 425.f);
        botonContinuar.setFillColor(sf::Color(245, 197, 66));
        botonContinuar.setOutlineColor(sf::Color::Black);
        botonContinuar.setOutlineThickness(2.f);
        ventana.draw(botonContinuar);
        ui.dibujarTexto(ventana, "CONTINUAR PARTIDO", panelX + 170.f, 439.f, 19, sf::Color::Black);
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

public:
    Juego() : ventana(sf::VideoMode(800, 600), "Futbol Adventure: Liga de Barrio"), estadoActual(GameState::MENU), musicaFondo(nullptr) {
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
        ultimoJefeDerrotado = 0;
        poderActivo = PowerUpType::NINGUNO;
        personajeSeleccionadoIdx = 0;
        opcionMenuHover = -1;
        personajeHover = -1;
        jefe = nullptr;

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

        // Cargar texturas de escenarios
        for(int i = 1; i <= 5; ++i) {
            tEscenarios[i-1] = Recursos::cargarEscenario(i);
        }

        tMenuFondo.loadFromFile("assets/images/Portadajuego.png");
        tMenuFondo.setSmooth(true);

        // Cargar sonidos
        bufLanzamiento = Recursos::cargarSonidoLanzamiento();
        bufSilbato = Recursos::cargarSonidoSilbato();
        bufEnemigoDerrota = Recursos::cargarSonidoEnemigoDerrota();
        bufDerrota = Recursos::cargarSonidoDerrota();
        bufVictoria = Recursos::cargarSonidoVictoria();

        sonidoLanzamiento.setBuffer(bufLanzamiento);
        sonidoLanzamiento.setVolume(30.f);
        sonidoSilbato.setBuffer(bufSilbato);
        sonidoSilbato.setVolume(30.f);
        sonidoEnemigoDerrota.setBuffer(bufEnemigoDerrota);
        sonidoEnemigoDerrota.setVolume(50.f);
        sonidoDerrota.setBuffer(bufDerrota);
        sonidoDerrota.setVolume(50.f);
        sonidoVictoria.setBuffer(bufVictoria);
        sonidoVictoria.setVolume(50.f);
        sonidoHover.setBuffer(bufSilbato);
        sonidoHover.setVolume(15.f);

        float tonosMuerte[5] = {0.82f, 0.95f, 1.08f, 1.22f, 1.38f};
        for (int i = 0; i < 5; ++i) {
            sonidosMuerteJefe[i].setBuffer(bufEnemigoDerrota);
            sonidosMuerteJefe[i].setVolume(55.f);
            sonidosMuerteJefe[i].setPitch(tonosMuerte[i]);
        }

        // Cargar música de fondo
        musicaFondo = Recursos::cargarMusicaFondo();
    }

    // METODO AJUSTADO A TU PIZARRON
    void Iniciar() {
        if (musicaFondo) {
            musicaFondo->play();
        }
        
        while (ventana.isOpen()) {
            float dt = relojFrame.restart().asSeconds();
            tiempoAnimacion += dt;
            if (estadoActual == GameState::JUGANDO) tiempoTotalJuego += dt;
            if (estadoActual == GameState::PANTALLA_NIVEL) tiempoTransicionNivel += dt;

            procesarEventos();
            actualizar(dt);
            renderizar();
        }
        
        if (musicaFondo) {
            musicaFondo->stop();
        }
    }

    ~Juego() { 
        delete jefe;
        if (musicaFondo) {
            musicaFondo->stop();
            delete musicaFondo;
        }
    }

private:
    void procesarEventos() {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) ventana.close();

            if (evento.type == sf::Event::MouseMoved) {
                actualizarHover(sf::Vector2f(static_cast<float>(evento.mouseMove.x), static_cast<float>(evento.mouseMove.y)));
            }

            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse(static_cast<float>(evento.mouseButton.x), static_cast<float>(evento.mouseButton.y));
                if (estadoActual == GameState::MENU) {
                    int idx = personajeEn(mouse);
                    if (idx >= 0) {
                        iniciarPartidaConPersonaje(idx);
                    } else {
                        int opcion = opcionMenuEn(mouse);
                        if (opcion >= 0) ejecutarOpcionMenu(opcion);
                    }
                } else if (estadoActual == GameState::GALERIA || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
                    estadoActual = GameState::MENU;
                } else if (estadoActual == GameState::PANTALLA_NIVEL) {
                    iniciarNivel(nivel);
                    estadoActual = GameState::JUGANDO;
                }
            }

            if (evento.type == sf::Event::KeyPressed) {
                if (estadoActual == GameState::MENU) {
                    if (evento.key.code >= sf::Keyboard::Num1 && evento.key.code <= sf::Keyboard::Num6) {
                        iniciarPartidaConPersonaje(evento.key.code - sf::Keyboard::Num1);
                    }
                    if (evento.key.code == sf::Keyboard::G) ejecutarOpcionMenu(0);
                    if (evento.key.code == sf::Keyboard::Escape) ejecutarOpcionMenu(1);
                }
                else if (estadoActual == GameState::GALERIA || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
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
                    if (evento.key.code == sf::Keyboard::Space) {
                        jugador.lanzarBalon(balones, tBalon);
                        sonidoLanzamiento.play();
                    }
                }
            }
        }
    }

    void actualizar(float dt) {
        if (estadoActual != GameState::JUGANDO) return;

        if (tiempoFlashJefe > 0.f) tiempoFlashJefe -= dt;
        if (tiempoSacudida > 0.f) tiempoSacudida -= dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) jugador.mover(-1.f, dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) jugador.mover(1.f, dt);

        jugador.actualizarFisica(dt);

        if (jefe) {
            jefe->actualizar(dt);
            jefe->atacar(dt, ataques, texturaProyectilEnemigo());
        }

        if (poderActivo != PowerUpType::NINGUNO) {
            tiempoPoder -= dt;
            if (tiempoPoder <= 0.f) {
                poderActivo = PowerUpType::NINGUNO;
                jugador.desactivarPowerUps();
            }
        }

        tiempoSpawnPowerUp += dt;
        if (tiempoSpawnPowerUp >= 6.5f) {
            tiempoSpawnPowerUp -= 6.5f;
            generarPowerUp();
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
                sonidoSilbato.play();
                tiempoFlashJefe = 0.18f;
                tiempoSacudida = 0.10f;
                if (jefe->vida <= 0) {
                    puntos += 20;
                    ultimoJefeDerrotado = std::max(0, std::min(4, nivel - 1));
                    sonidosMuerteJefe[ultimoJefeDerrotado].play();
                    if (nivel < 5) {
                        nivel++;
                        tiempoTransicionNivel = 0.f;
                        estadoActual = GameState::PANTALLA_NIVEL;
                        return;
                    } else {
                        estadoActual = GameState::VICTORIA;
                        sonidoVictoria.play();
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
                if (!jugador.invencible) {
                    jugador.vidas--;
                    if (jugador.vidas <= 0) {
                        estadoActual = GameState::GAMEOVER;
                        sonidoDerrota.play();
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
            if (it->sprite.getGlobalBounds().intersects(jugador.sprite.getGlobalBounds())) {
                poderActivo = it->tipo;
                if(poderActivo == PowerUpType::TARJETA_ROJA) {
                    estadoActual = GameState::GAMEOVER;
                    sonidoDerrota.play();
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
            } else {
                ++it;
            }
        }
    }

    void renderizar() {
        ventana.clear(sf::Color(34, 139, 34)); 

        if (estadoActual == GameState::MENU) {
            dibujarFondoMenu();
            dibujarPanelMenu();

            ui.dibujarTexto(ventana, "ARCADE 2D - COPA DE BARRIO", 56, 62, 16, sf::Color(220, 255, 225));
            dibujarTextoSombra("THE LAST", 56, 108, 54, sf::Color::White);
            dibujarTextoSombra("DANCE", 56, 160, 64, sf::Color(245, 210, 65));
            ui.dibujarTexto(ventana, "Futbol Mexicano Multiverse", 60, 234, 22, sf::Color(110, 245, 150));

            dibujarInstruccionesPortada();
            dibujarSelectorPersonajesPortada();
            dibujarItemsPortada();

            dibujarBotonMenu(0, "GALERIA DE RIVALES", 396, sf::Color(80, 210, 255));
            dibujarBotonMenu(1, "SALIR DEL ESTADIO", 464, sf::Color(255, 90, 80));

            ui.dibujarTexto(ventana, "G: galeria  |  ESC: salir", 58, 536, 14, sf::Color(230, 230, 230));
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

                ui.dibujarTexto(ventana, "NIVEL " + std::to_string(i + 1), 128, y + 12, 17, sf::Color(245, 210, 65));
                ui.dibujarTexto(ventana, nombresEnemigos[i], 245, y + 12, 18, sf::Color::White);
                ui.dibujarTexto(ventana, "Ataque: " + ataquesTxt[i], 500, y + 12, 16, sf::Color(110, 245, 150));
            }

            ui.dibujarTexto(ventana, "ENTER / ESC para regresar", 285, 500, 18, sf::Color(245, 210, 65));
        }
        else if (estadoActual == GameState::JUGANDO) {
            // Dibujar escenario de fondo
            if (nivel >= 1 && nivel <= 5) {
                escenarioActual.setTexture(tEscenarios[nivel - 1]);
                escenarioActual.setScale(
                    800.f / escenarioActual.getLocalBounds().width,
                    600.f / escenarioActual.getLocalBounds().height
                );
                escenarioActual.setPosition(0, 0);
                ventana.draw(escenarioActual);
            }
            
            ventana.draw(jugador.sprite);
            if (jefe) {
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
