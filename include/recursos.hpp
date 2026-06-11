#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class Recursos {
public:
    // Texturas de personajes
    static sf::Texture cargarTexturaPersonaje(int idx) {
        sf::Texture tex;
        std::string nombres[6] = {
            "assets/images/chicharron/chicharronparado.png",
            "assets/images/cuau/cuauparado.png",
            "assets/images/funesmorri/funesmorriparado.png",
            "assets/images/gino/ginoparado.png",
            "assets/images/lugosanchez/lugosanchezmovimientos.png",
            "assets/images/chaquetagimenez/chaquetagimenezparado.png"
        };
        if (idx >= 0 && idx < 6) {
            tex.loadFromFile(nombres[idx]);
        }
        return tex;
    }

    // Texturas de enemigos
    static sf::Texture cargarTexturaEnemigo(int nivel) {
        sf::Texture tex;
        std::string nombres[5] = {
            "assets/images/enemigos/katie itzel/katieitzelreal.png",
            "assets/images/enemigos/gata ortencia/gataortenciareal.png",
            "assets/images/enemigos/telecomeriales/telecomercialesreal.png",
            "assets/images/enemigos/funko arreola/funkorarreolareal.png",
            "assets/images/enemigos/mafia mayor/mafiamayorreal.png"
        };
        if (nivel >= 1 && nivel <= 5) {
            tex.loadFromFile(nombres[nivel - 1]);
        }
        return tex;
    }

    // Texturas de proyectiles y power-ups
    static sf::Texture cargarTexturaBalon() {
        sf::Texture tex;
        tex.create(20, 20);
        return tex;
    }

    static sf::Texture cargarTexturaPowerUp(const std::string& tipo) {
        sf::Texture tex;
        tex.create(25, 25);
        return tex;
    }

    // Música
    static sf::Music* cargarMusicaFondo() {
        sf::Music* musica = new sf::Music();
        musica->openFromFile("assets/music/musicadefondo/Canciodefondojuego.ogg");
        musica->setLoop(true);
        musica->setVolume(50.f);
        return musica;
    }

    static sf::Music* cargarMusicaIntro() {
        sf::Music* musica = new sf::Music();
        musica->openFromFile("assets/music/musicadefondo/Intro.ogg");
        musica->setVolume(50.f);
        return musica;
    }

    // Sound Buffers (efectos)
    static sf::SoundBuffer cargarSonidoLanzamiento() {
        sf::SoundBuffer buf;
        buf.loadFromFile("assets/music/Sonidosjuego/Lanzamiento.ogg");
        return buf;
    }

    static sf::SoundBuffer cargarSonidoSilbato() {
        sf::SoundBuffer buf;
        buf.loadFromFile("assets/music/Sonidosjuego/silbato.ogg");
        return buf;
    }

    static sf::SoundBuffer cargarSonidoEnemigoDerrota() {
        sf::SoundBuffer buf;
        buf.loadFromFile("assets/music/Sonidosjuego/enemigoderrotado.ogg");
        return buf;
    }

    static sf::SoundBuffer cargarSonidoDerrota() {
        sf::SoundBuffer buf;
        buf.loadFromFile("assets/music/Sonidosjuego/derrotado.ogg");
        return buf;
    }

    static sf::SoundBuffer cargarSonidoVictoria() {
        sf::SoundBuffer buf;
        buf.loadFromFile("assets/music/Sonidosjuego/Juegoganado.ogg");
        return buf;
    }

    // Escenarios
    static sf::Texture cargarEscenario(int nivel) {
        sf::Texture tex;
        std::string nombres[5] = {
            "assets/images/Escenarios/Escenario1.png",
            "assets/images/Escenarios/Escenario2.png",
            "assets/images/Escenarios/Escenario3.png",
            "assets/images/Escenarios/escenario4.png",
            "assets/images/Escenarios/Escenario5.png"
        };
        if (nivel >= 1 && nivel <= 5) {
            tex.loadFromFile(nombres[nivel - 1]);
        }
        return tex;
    }
};
