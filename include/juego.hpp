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

    // Audio
    sf::Music* musicaFondo;
    sf::Sound sonidoLanzamiento, sonidoSilbato, sonidoEnemigoDerrota, sonidoDerrota, sonidoVictoria;
    sf::SoundBuffer bufLanzamiento, bufSilbato, bufEnemigoDerrota, bufDerrota, bufVictoria;

    // Sprites de escenarios
    sf::Sprite escenarioActual;

    int puntos;
    int nivel;
    float tiempoPoder;
    PowerUpType poderActivo;
    sf::Clock relojFrame;
    float tiempoTotalJuego;

    std::string nombresEnemigos[5] = {"Katie Itzel", "Gata Ortencia", "Telecomerciales", "Funko Arreola", "Mafia Mayor"};
    std::string nombresPersonajes[6] = {"Chicharron", "Cuau", "Funesmorri", "Gino", "Lugo Sanchez", "Chaqueta Gimenez"};
    int personajeSeleccionadoIdx;

    void iniciarNivel(int nvl) {
        delete jefe;
        jefe = new Enemigo(nombresEnemigos[nvl-1], nvl, tEnemigos[nvl-1]);
        balones.clear();
        ataques.clear();
        powerups.clear();
        jugador.desactivarPowerUps();
        poderActivo = PowerUpType::NINGUNO;
    }

public:
    Juego() : ventana(sf::VideoMode(800, 600), "Futbol Adventure: Liga de Barrio"), estadoActual(GameState::MENU), musicaFondo(nullptr) {
        std::srand(std::time(nullptr));
        puntos = 0;
        nivel = 1;
        tiempoPoder = 0.f;
        tiempoTotalJuego = 0.f;
        poderActivo = PowerUpType::NINGUNO;
        personajeSeleccionadoIdx = 0;
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
            if (estadoActual == GameState::JUGANDO) tiempoTotalJuego += dt;

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

            if (evento.type == sf::Event::KeyPressed) {
                if (estadoActual == GameState::MENU) {
                    if (evento.key.code == sf::Keyboard::Num1) estadoActual = GameState::SELECCION;
                    if (evento.key.code == sf::Keyboard::Num2) estadoActual = GameState::GALERIA;
                    if (evento.key.code == sf::Keyboard::Num3) ventana.close();
                }
                else if (estadoActual == GameState::SELECCION) {
                    if (evento.key.code >= sf::Keyboard::Num0 && evento.key.code <= sf::Keyboard::Num5) {
                        personajeSeleccionadoIdx = evento.key.code - sf::Keyboard::Num0;
                        // Cargar textura del personaje usando el sistema de recursos
                        jugador.textura = Recursos::cargarTexturaPersonaje(personajeSeleccionadoIdx);
                        jugador.sprite.setTexture(jugador.textura);
                        jugador.sprite.setOrigin(jugador.textura.getSize().x / 2.f, jugador.textura.getSize().y / 2.f);
                        jugador.sprite.setPosition(jugador.x, jugador.y);
                        jugador.sprite.setScale(0.2f, 0.2f);
                        
                        nivel = 1; puntos = 0; jugador.vidas = 3; tiempoTotalJuego = 0.f;
                        iniciarNivel(nivel);
                        estadoActual = GameState::JUGANDO;
                    }
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) jugador.mover(-1.f, dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) jugador.mover(1.f, dt);

        jugador.actualizarFisica(dt);

        if (jefe) {
            jefe->actualizar(dt);
            sf::Texture& proyectilActual = (nivel == 3 || nivel == 5) ? tDinero : ((nivel == 4) ? tFlecha : tSilbato);
            jefe->atacar(dt, ataques, proyectilActual);
        }

        if (poderActivo != PowerUpType::NINGUNO) {
            tiempoPoder -= dt;
            if (tiempoPoder <= 0.f) {
                poderActivo = PowerUpType::NINGUNO;
                jugador.desactivarPowerUps();
            }
        }

        if (std::rand() % 400 == 7) {
            PowerUpType randomTipo = static_cast<PowerUpType>((std::rand() % 6) + 1);
            sf::Texture& tex = (randomTipo == PowerUpType::TAQUETES) ? tTaquetes : tEspinilleras;
            powerups.push_back(PowerUp(std::rand() % 700 + 50, 0.f, randomTipo, tex));
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
                if (jefe->vida <= 0) {
                    puntos += 20;
                    sonidoEnemigoDerrota.play();
                    if (nivel < 5) {
                        nivel++;
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
            ui.dibujarTexto(ventana, "FUTBOL MEXICANO MULTIVERSE", 120, 100, 35, sf::Color::White);
            ui.dibujarTexto(ventana, "[1] JUGAR", 320, 250, 25, sf::Color::Yellow);
            ui.dibujarTexto(ventana, "[2] VER GALERIA DE ENEMIGOS", 220, 320, 25, sf::Color::Cyan);
            ui.dibujarTexto(ventana, "[3] SALIR", 320, 390, 25, sf::Color::Red);
        } 
        else if (estadoActual == GameState::SELECCION) {
            ui.dibujarTexto(ventana, "SELECCIONA TU FUTBOLISTA", 180, 50, 30, sf::Color::White);
            for(int i = 0; i < 6; ++i) {
                ui.dibujarTexto(ventana, "[" + std::to_string(i) + "] " + nombresPersonajes[i], 150, 150 + (i * 50), 22, sf::Color::Yellow);
            }
        }
        else if (estadoActual == GameState::GALERIA) {
            ui.dibujarTexto(ventana, "GALERIA DE IMPRESENTABLES (RIVALES)", 120, 30, 28, sf::Color::Magenta);
            ui.dibujarTexto(ventana, "Nvl 1: Katie Itzel - Lanza Silbatos", 100, 120, 20, sf::Color::White);
            ui.dibujarTexto(ventana, "Nvl 2: Gata Ortencia - Lanza Silbatos", 100, 180, 20, sf::Color::White);
            ui.dibujarTexto(ventana, "Nvl 3: Telecomerciales - Lanza Dinero", 100, 240, 20, sf::Color::White);
            ui.dibujarTexto(ventana, "Nvl 4: Funko Arreola - Lanza Flechas Ascenso", 100, 300, 20, sf::Color::White);
            ui.dibujarTexto(ventana, "Nvl 5: Mafia Mayor - Lanza Billetes", 100, 360, 20, sf::Color::White);
            ui.dibujarTexto(ventana, "Presiona [ENTER] para regresar", 220, 480, 20, sf::Color::Yellow);
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
            if (jefe) ventana.draw(jefe->sprite);
            for (auto& b : balones) ventana.draw(b.sprite);
            for (auto& a : ataques) ventana.draw(a.sprite);
            for (auto& p : powerups) ventana.draw(p.sprite);

            ui.dibujarHUD(ventana, puntos, nivel, jugador.vidas, (jefe ? jefe->vida : 0), poderActivo, tiempoPoder);
        }
        else if (estadoActual == GameState::PANTALLA_NIVEL) {
            ui.dibujarTexto(ventana, "NIVEL COMPLETADO!", 240, 150, 35, sf::Color::Green);
            ui.dibujarTexto(ventana, "Puntos Acumulados: " + std::to_string(puntos), 260, 230, 22, sf::Color::White);
            ui.dibujarTexto(ventana, "Vidas Restantes: " + std::to_string(jugador.vidas), 260, 280, 22, sf::Color::White);
            if(nivel <= 5) ui.dibujarTexto(ventana, "Proximo Rival: " + nombresEnemigos[nivel-1], 260, 330, 22, sf::Color::Yellow);
            ui.dibujarTexto(ventana, "PRESIONA [ESPACIO] PARA CONTINUAR", 180, 430, 22, sf::Color::Cyan);
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