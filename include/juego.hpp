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

enum class GameState { MENU, SELECCION, GALERIA, JUGANDO, PANTALLA_NIVEL, VICTORIA, GAMEOVER };

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
    sf::Music* musicaMenu;
    bool musicaJuegoActiva;
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

    void dibujarFondoMenu() {
        if (tMenuFondo.getSize().x > 0 && tMenuFondo.getSize().y > 0) {
            menuFondo.setTexture(tMenuFondo);
            float scaleX = 800.f / static_cast<float>(tMenuFondo.getSize().x);
            float scaleY = 600.f / static_cast<float>(tMenuFondo.getSize().y);
            float scale = (scaleX > scaleY ? scaleX : scaleY) * (1.04f + std::sin(tiempoAnimacion * 0.45f) * 0.018f);
            float movimientoX = std::sin(tiempoAnimacion * 0.25f) * 18.f;
            float movimientoY = std::cos(tiempoAnimacion * 0.20f) * 10.f;
            menuFondo.setScale(scale, scale);
            menuFondo.setPosition(
                (800.f - tMenuFondo.getSize().x * scale) / 2.f + movimientoX,
                (600.f - tMenuFondo.getSize().y * scale) / 2.f + movimientoY
            );
            ventana.draw(menuFondo);
        } else {
            ventana.clear(sf::Color(20, 90, 45));
        }

        sf::RectangleShape capa(sf::Vector2f(800.f, 600.f));
        capa.setFillColor(sf::Color(0, 0, 0, 48));
        ventana.draw(capa);

        sf::RectangleShape sombraSuperior(sf::Vector2f(800.f, 155.f));
        sombraSuperior.setPosition(0.f, 0.f);
        sombraSuperior.setFillColor(sf::Color(0, 0, 0, 58));
        ventana.draw(sombraSuperior);

        sf::RectangleShape franjaInferior(sf::Vector2f(800.f, 150.f));
        franjaInferior.setPosition(0.f, 450.f);
        franjaInferior.setFillColor(sf::Color(0, 0, 0, 88));
        ventana.draw(franjaInferior);
    }

    void dibujarTextoSombra(const std::string& texto, float x, float y, int tam, sf::Color color) {
        ui.dibujarTexto(ventana, texto, x + 3.f, y + 3.f, tam, sf::Color(0, 0, 0, 190));
        ui.dibujarTexto(ventana, texto, x, y, tam, color);
    }

    sf::FloatRect rectBotonMenu(int opcion) {
        return sf::FloatRect(430.f, 325.f + opcion * 58.f, 315.f, 48.f);
    }

    sf::FloatRect rectPersonaje(int idx) {
        int col = idx % 2;
        int row = idx / 2;
        return sf::FloatRect(105.f + col * 330.f, 185.f + row * 82.f, 275.f, 58.f);
    }

    int opcionMenuEn(sf::Vector2f punto) {
        for (int i = 0; i < 3; ++i) {
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
            reproducirHoverSiCambio(opcionMenuEn(punto), opcionMenuHover);
            personajeHover = -1;
        } else if (estadoActual == GameState::SELECCION) {
            reproducirHoverSiCambio(personajeEn(punto), personajeHover);
            opcionMenuHover = -1;
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
        if (opcion == 0) estadoActual = GameState::SELECCION;
        else if (opcion == 1) estadoActual = GameState::GALERIA;
        else if (opcion == 2) ventana.close();
        opcionMenuHover = -1;
        personajeHover = -1;
    }

    void dibujarBotonMenu(int opcion, const std::string& texto, float y, sf::Color color) {
        bool hover = opcionMenuHover == opcion;
        sf::FloatRect rect = rectBotonMenu(opcion);
        float latido = (std::sin(tiempoAnimacion * 6.f + opcion) + 1.f) * 0.5f;

        sf::RectangleShape sombra(sf::Vector2f(rect.width, rect.height));
        sombra.setPosition(rect.left + 8.f, rect.top + 9.f);
        sombra.setFillColor(sf::Color(0, 0, 0, 150));
        ventana.draw(sombra);

        if (hover) {
            sf::RectangleShape brillo(sf::Vector2f(rect.width + 18.f, rect.height + 14.f));
            brillo.setPosition(rect.left - 9.f, rect.top - 7.f);
            brillo.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(32 + latido * 35)));
            ventana.draw(brillo);
        }

        sf::RectangleShape boton(sf::Vector2f(rect.width, rect.height));
        boton.setPosition(rect.left, rect.top);
        boton.setFillColor(hover ? sf::Color(18, 38, 34, 236) : sf::Color(6, 17, 18, 188));
        boton.setOutlineColor(hover ? sf::Color::White : color);
        boton.setOutlineThickness(hover ? 3.f : 2.f);
        ventana.draw(boton);

        sf::RectangleShape linea(sf::Vector2f(7.f, rect.height));
        linea.setPosition(rect.left, rect.top);
        linea.setFillColor(color);
        ventana.draw(linea);

        sf::CircleShape balon(13.f);
        balon.setPosition(rect.left + 23.f, rect.top + 11.f);
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

        ui.dibujarTexto(ventana, texto, rect.left + 66.f, y + 13.f, 18, hover ? sf::Color(245, 245, 245) : sf::Color::White);
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
        sf::RectangleShape marcador(sf::Vector2f(360.f, 54.f));
        marcador.setPosition(42.f, 505.f);
        marcador.setFillColor(sf::Color(4, 12, 18, 205));
        marcador.setOutlineColor(sf::Color(245, 210, 65, 210));
        marcador.setOutlineThickness(2.f);
        ventana.draw(marcador);

        ui.dibujarTexto(ventana, "5 NIVELES", 72.f, 516.f, 17, sf::Color(245, 210, 65));
        ui.dibujarTexto(ventana, "5 RIVALES", 190.f, 516.f, 17, sf::Color(80, 210, 255));
        ui.dibujarTexto(ventana, "100 PTS", 305.f, 516.f, 17, sf::Color(110, 245, 150));
        ui.dibujarTexto(ventana, "Mueve el cursor y haz click para elegir.", 68.f, 543.f, 13, sf::Color(230, 230, 230));
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
    Juego() : ventana(sf::VideoMode(800, 600), "Futbol Adventure: Liga de Barrio"), estadoActual(GameState::MENU), musicaFondo(nullptr), musicaMenu(nullptr), musicaJuegoActiva(false) {
        std::srand(std::time(nullptr));
        puntos = 0;
        nivel = 1;
        tiempoPoder = 0.f;
        tiempoTotalJuego = 0.f;
        tiempoAnimacion = 0.f;
        tiempoTransicionNivel = 0.f;
        tiempoFlashJefe = 0.f;
        tiempoSacudida = 0.f;
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

        // Cargar música
        musicaFondo = Recursos::cargarMusicaFondo();
        musicaMenu = Recursos::cargarMusicaIntro();
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
                } else if (estadoActual == GameState::SELECCION) {
                    int idx = personajeEn(mouse);
                    if (idx >= 0) iniciarPartidaConPersonaje(idx);
                } else if (estadoActual == GameState::GALERIA || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
                    estadoActual = GameState::MENU;
                } else if (estadoActual == GameState::PANTALLA_NIVEL) {
                    iniciarNivel(nivel);
                    estadoActual = GameState::JUGANDO;
                }
            }

            if (evento.type == sf::Event::KeyPressed) {
                if (estadoActual == GameState::GALERIA || estadoActual == GameState::VICTORIA || estadoActual == GameState::GAMEOVER) {
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

        int probabilidadPowerUp = 950 + (nivel * 120);
        if (powerups.empty() && std::rand() % probabilidadPowerUp == 7) {
            PowerUpType randomTipo = static_cast<PowerUpType>((std::rand() % 6) + 1);
            powerups.push_back(PowerUp(std::rand() % 700 + 50, 0.f, randomTipo, texturaParaPowerUp(randomTipo)));
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
            dibujarEfectosPortada();

            sf::RectangleShape etiqueta(sf::Vector2f(265.f, 44.f));
            etiqueta.setPosition(42.f, 42.f);
            etiqueta.setFillColor(sf::Color(4, 12, 18, 190));
            etiqueta.setOutlineColor(sf::Color(80, 210, 255, 190));
            etiqueta.setOutlineThickness(2.f);
            ventana.draw(etiqueta);

            sf::CircleShape puntoVivo(8.f);
            puntoVivo.setPosition(58.f, 56.f);
            puntoVivo.setFillColor(sf::Color(80, 230, 130));
            ventana.draw(puntoVivo);

            ui.dibujarTexto(ventana, "COPA DE BARRIO", 82, 51, 18, sf::Color::White);
            ui.dibujarTexto(ventana, "Futbol Mexicano Multiverse", 432, 292, 18, sf::Color(110, 245, 150));

            sf::RectangleShape menuBase(sf::Vector2f(340.f, 218.f));
            menuBase.setPosition(415.f, 307.f);
            menuBase.setFillColor(sf::Color(0, 0, 0, 62));
            menuBase.setOutlineColor(sf::Color(255, 255, 255, 45));
            menuBase.setOutlineThickness(1.f);
            ventana.draw(menuBase);

            sf::RectangleShape linea(sf::Vector2f(230.f, 5.f));
            linea.setPosition(430.f, 315.f);
            linea.setFillColor(sf::Color(50, 210, 110));
            ventana.draw(linea);

            dibujarBotonMenu(0, "JUGAR PARTIDO", 325, sf::Color(245, 210, 65));
            dibujarBotonMenu(1, "GALERIA DE RIVALES", 383, sf::Color(80, 210, 255));
            dibujarBotonMenu(2, "SALIR DEL ESTADIO", 441, sf::Color(255, 90, 80));

            dibujarResumenPortada();
        } 
        else if (estadoActual == GameState::SELECCION) {
            dibujarFondoMenu();
            sf::RectangleShape panel(sf::Vector2f(690.f, 500.f));
            panel.setPosition(55.f, 55.f);
            panel.setFillColor(sf::Color(0, 0, 0, 160));
            panel.setOutlineColor(sf::Color(245, 210, 65));
            panel.setOutlineThickness(2.f);
            ventana.draw(panel);

            dibujarTextoSombra("SELECCIONA TU FUTBOLISTA", 115, 82, 34, sf::Color::White);
            ui.dibujarTexto(ventana, "La eleccion cambia la apariencia, la habilidad depende de ti.", 120, 130, 16, sf::Color(210, 240, 220));

            for(int i = 0; i < 6; ++i) {
                sf::FloatRect rect = rectPersonaje(i);
                bool hover = personajeHover == i;

                sf::RectangleShape sombra(sf::Vector2f(rect.width, rect.height));
                sombra.setPosition(rect.left + 5.f, rect.top + 6.f);
                sombra.setFillColor(sf::Color(0, 0, 0, 140));
                ventana.draw(sombra);

                sf::RectangleShape card(sf::Vector2f(rect.width, rect.height));
                card.setPosition(rect.left, rect.top);
                card.setFillColor(hover ? sf::Color(28, 58, 45, 245) : sf::Color(15, 28, 28, 225));
                card.setOutlineColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
                card.setOutlineThickness(hover ? 3.f : 2.f);
                ventana.draw(card);

                sf::CircleShape emblema(18.f);
                emblema.setPosition(rect.left + 14.f, rect.top + 11.f);
                emblema.setFillColor(hover ? sf::Color(245, 210, 65) : sf::Color(60, 200, 115));
                emblema.setOutlineColor(sf::Color::Black);
                emblema.setOutlineThickness(2.f);
                ventana.draw(emblema);

                ui.dibujarTexto(ventana, "FC", rect.left + 20.f, rect.top + 18.f, 13, sf::Color::Black);
                ui.dibujarTexto(ventana, nombresPersonajes[i], rect.left + 68.f, rect.top + 18.f, 18, sf::Color::White);

                if (hover) {
                    ui.dibujarTexto(ventana, "Elegir", rect.left + 205.f, rect.top + 20.f, 14, sf::Color(245, 210, 65));
                }
            }

            ui.dibujarTexto(ventana, "Elige tu futbolista con el cursor.", 245, 500, 16, sf::Color(245, 210, 65));
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
