#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <algorithm>
#include <queue>
#include <vector>

class Recursos {
public:
    static bool esColorFondoSprite(sf::Color color) {
        if (color.a == 0) return true;

        int maximo = std::max(static_cast<int>(color.r), std::max(static_cast<int>(color.g), static_cast<int>(color.b)));
        int minimo = std::min(static_cast<int>(color.r), std::min(static_cast<int>(color.g), static_cast<int>(color.b)));
        int promedio = (static_cast<int>(color.r) + static_cast<int>(color.g) + static_cast<int>(color.b)) / 3;

        return (maximo - minimo <= 24 && promedio >= 45);
    }

    static void quitarFondoSprite(sf::Image& imagen) {
        sf::Vector2u size = imagen.getSize();
        if (size.x == 0 || size.y == 0) return;

        std::vector<bool> visitado(size.x * size.y, false);
        std::queue<sf::Vector2u> pendientes;

        auto intentarAgregar = [&](unsigned int x, unsigned int y) {
            if (x >= size.x || y >= size.y) return;

            unsigned int pos = y * size.x + x;
            if (visitado[pos] || !esColorFondoSprite(imagen.getPixel(x, y))) return;

            visitado[pos] = true;
            pendientes.push(sf::Vector2u(x, y));
        };

        for (unsigned int x = 0; x < size.x; ++x) {
            intentarAgregar(x, 0);
            intentarAgregar(x, size.y - 1);
        }

        for (unsigned int y = 0; y < size.y; ++y) {
            intentarAgregar(0, y);
            intentarAgregar(size.x - 1, y);
        }

        while (!pendientes.empty()) {
            sf::Vector2u punto = pendientes.front();
            pendientes.pop();

            imagen.setPixel(punto.x, punto.y, sf::Color(255, 255, 255, 0));

            if (punto.x > 0) intentarAgregar(punto.x - 1, punto.y);
            if (punto.x + 1 < size.x) intentarAgregar(punto.x + 1, punto.y);
            if (punto.y > 0) intentarAgregar(punto.x, punto.y - 1);
            if (punto.y + 1 < size.y) intentarAgregar(punto.x, punto.y + 1);
        }
    }

    static sf::Texture cargarTexturaSpriteSinFondo(const std::string& path) {
        sf::Texture tex;
        sf::Image imagen;

        if (imagen.loadFromFile(path)) {
            quitarFondoSprite(imagen);
            tex.loadFromImage(imagen);
        }

        tex.setSmooth(false);
        return tex;
    }

    static sf::Texture cargarTexturaSpriteSinFondo(const std::string& path, const sf::IntRect& area) {
        sf::Texture tex;
        sf::Image imagen;

        if (imagen.loadFromFile(path)) {
            quitarFondoSprite(imagen);
            tex.loadFromImage(imagen, area);
        }

        tex.setSmooth(false);
        return tex;
    }

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
        if (idx == 4) {
            tex = cargarTexturaSpriteSinFondo(nombres[idx], sf::IntRect(35, 55, 175, 245));
        } else if (idx >= 0 && idx < 6) {
            tex = cargarTexturaSpriteSinFondo(nombres[idx]);
        }
        return tex;
    }

    // Texturas de enemigos
    static sf::Texture cargarTexturaEnemigo(int nivel) {
        sf::Texture tex;
        std::string nombres[5] = {
            "assets/images/enemigos/katie itzel/sprite katieitzel/katieitzel.png",
            "assets/images/enemigos/gata ortencia/sprite gataortencia/gataortencia.png",
            "assets/images/enemigos/telecomeriales/sprite telecomerciales/telecomerciales.png",
            "assets/images/enemigos/funko arreola/sprite funkoarreola/funkoarreola.png",
            "assets/images/enemigos/mafia mayor/sprite mafiamayor/mafiamayor.png"
        };
        if (nivel >= 1 && nivel <= 5) {
            tex = cargarTexturaSpriteSinFondo(nombres[nivel - 1]);
        }
        return tex;
    }

    // Texturas de proyectiles y power-ups
    static sf::Texture cargarTexturaBalon() {
        sf::Texture tex;
        if (!tex.loadFromFile("assets/images/items/balon.png")) {
            tex.create(20, 20);
        }
        tex.setSmooth(true);
        return tex;
    }

    static sf::Texture cargarTexturaPowerUp(const std::string& tipo) {
        sf::Texture tex;
        std::string path = "assets/images/items/" + tipo + ".png";
        if (!tex.loadFromFile(path)) {
            tex.create(25, 25);
        }
        tex.setSmooth(true);
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
        musica->setLoop(true);
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
