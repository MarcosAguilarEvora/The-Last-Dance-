#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <algorithm>
#include <queue>
#include <vector>
#include <climits>
#include <fstream>

class Recursos {
public:
    static bool existeArchivo(const std::string& path) {
        std::ifstream archivo(path.c_str(), std::ios::binary);
        return archivo.good();
    }

    static std::string rutaArchivo(const std::string& path) {
        if (existeArchivo(path)) return path;

        std::string desdeBin = "../" + path;
        if (existeArchivo(desdeBin)) return desdeBin;

        std::string desdeSubcarpeta = "../../" + path;
        if (existeArchivo(desdeSubcarpeta)) return desdeSubcarpeta;

        return path;
    }

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

        if (imagen.loadFromFile(rutaArchivo(path))) {
            quitarFondoSprite(imagen);
            tex.loadFromImage(imagen);
        }

        tex.setSmooth(false);
        return tex;
    }

    static sf::Texture cargarTexturaSpriteSinFondo(const std::string& path, const sf::IntRect& area) {
        sf::Texture tex;
        sf::Image imagen;

        if (imagen.loadFromFile(rutaArchivo(path))) {
            quitarFondoSprite(imagen);
            tex.loadFromImage(imagen, area);
        }

        tex.setSmooth(false);
        return tex;
    }

    static bool pixelVisible(const sf::Image& imagen, unsigned int x, unsigned int y) {
        return imagen.getPixel(x, y).a > 20;
    }

    static sf::Texture cargarTexturaSpriteSheetSinFondo(const std::string& path, int columnas, const sf::IntRect& area = sf::IntRect()) {
        sf::Texture tex;
        sf::Image imagen;

        if (!imagen.loadFromFile(rutaArchivo(path)) || columnas <= 0) {
            tex.create(1, 1);
            tex.setSmooth(false);
            return tex;
        }

        quitarFondoSprite(imagen);

        sf::IntRect zona = area;
        if (zona.width <= 0 || zona.height <= 0) {
            zona = sf::IntRect(0, 0, static_cast<int>(imagen.getSize().x), static_cast<int>(imagen.getSize().y));
        }

        int anchoCelda = zona.width / columnas;
        int altoCelda = zona.height;
        std::vector<sf::IntRect> recortes;
        int maxW = 1;
        int maxH = 1;

        for (int i = 0; i < columnas; ++i) {
            int inicioX = zona.left + i * anchoCelda;
            int finX = (i == columnas - 1) ? zona.left + zona.width : inicioX + anchoCelda;
            int minX = INT_MAX;
            int minY = INT_MAX;
            int maxX = -1;
            int maxY = -1;

            for (int y = zona.top; y < zona.top + altoCelda; ++y) {
                for (int x = inicioX; x < finX; ++x) {
                    if (x < 0 || y < 0 || x >= static_cast<int>(imagen.getSize().x) || y >= static_cast<int>(imagen.getSize().y)) continue;
                    if (pixelVisible(imagen, static_cast<unsigned int>(x), static_cast<unsigned int>(y))) {
                        minX = std::min(minX, x);
                        minY = std::min(minY, y);
                        maxX = std::max(maxX, x);
                        maxY = std::max(maxY, y);
                    }
                }
            }

            if (maxX < minX || maxY < minY) {
                recortes.push_back(sf::IntRect(inicioX, zona.top, anchoCelda, altoCelda));
            } else {
                int margen = 4;
                minX = std::max(zona.left, minX - margen);
                minY = std::max(zona.top, minY - margen);
                maxX = std::min(zona.left + zona.width - 1, maxX + margen);
                maxY = std::min(zona.top + zona.height - 1, maxY + margen);
                recortes.push_back(sf::IntRect(minX, minY, maxX - minX + 1, maxY - minY + 1));
                maxW = std::max(maxW, maxX - minX + 1);
                maxH = std::max(maxH, maxY - minY + 1);
            }
        }

        sf::Image hojaNormalizada;
        hojaNormalizada.create(maxW * columnas, maxH, sf::Color(255, 255, 255, 0));

        for (int i = 0; i < columnas; ++i) {
            sf::IntRect r = recortes[i];
            int offsetX = i * maxW + (maxW - r.width) / 2;
            int offsetY = maxH - r.height;

            for (int y = 0; y < r.height; ++y) {
                for (int x = 0; x < r.width; ++x) {
                    sf::Color pixel = imagen.getPixel(static_cast<unsigned int>(r.left + x), static_cast<unsigned int>(r.top + y));
                    hojaNormalizada.setPixel(static_cast<unsigned int>(offsetX + x), static_cast<unsigned int>(offsetY + y), pixel);
                }
            }
        }

        tex.loadFromImage(hojaNormalizada);
        tex.setSmooth(false);
        return tex;
    }

    // Texturas de personajes
    static sf::Texture cargarTexturaPersonaje(int idx) {
        sf::Texture tex;
        std::string nombres[6] = {
            "assets/images/chicharron/chicharronderecha.png",
            "assets/images/cuau/cuauderecha.png",
            "assets/images/funesmorri/funesmorriderecha.png",
            "assets/images/gino/ginoderecha.png",
            "assets/images/lugosanchez/lugosanchezmovimientos.png",
            "assets/images/chaquetagimenez/chaquetagimenezizquierdaderecha.png"
        };
        int columnas[6] = {4, 4, 4, 4, 5, 4};
        (void)columnas;
        if (idx >= 0 && idx < 6) {
            tex.loadFromFile(rutaArchivo(nombres[idx]));
            tex.setSmooth(false);
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
            tex.loadFromFile(rutaArchivo(nombres[nivel - 1]));
            tex.setSmooth(false);
        }
        return tex;
    }

    // Texturas de proyectiles y power-ups
    static sf::Texture cargarTexturaBalon() {
        sf::Texture tex;
        if (!tex.loadFromFile(rutaArchivo("assets/images/items/balon.png"))) {
            tex.create(20, 20);
        }
        tex.setSmooth(true);
        return tex;
    }

    static sf::Texture cargarTexturaPowerUp(const std::string& tipo) {
        sf::Texture tex;
        std::string path = "assets/images/items/" + tipo + ".png";
        if (!tex.loadFromFile(rutaArchivo(path))) {
            tex.create(25, 25);
        }
        tex.setSmooth(true);
        return tex;
    }

    // Música
    static sf::Music* cargarMusicaFondo() {
        sf::Music* musica = new sf::Music();
        musica->openFromFile(rutaArchivo("assets/music/musicadefondo/Canciodefondojuego.ogg"));
        musica->setLoop(true);
        musica->setVolume(50.f);
        return musica;
    }

    static sf::Music* cargarMusicaIntro() {
        sf::Music* musica = new sf::Music();
        musica->openFromFile(rutaArchivo("assets/music/musicadefondo/Intro.ogg"));
        musica->setLoop(true);
        musica->setVolume(50.f);
        return musica;
    }

    // Sound Buffers (efectos)
    static sf::SoundBuffer cargarSonidoLanzamiento() {
        sf::SoundBuffer buf;
        buf.loadFromFile(rutaArchivo("assets/music/Sonidosjuego/Lanzamiento.ogg"));
        return buf;
    }

    static sf::SoundBuffer cargarSonidoSilbato() {
        sf::SoundBuffer buf;
        buf.loadFromFile(rutaArchivo("assets/music/Sonidosjuego/silbato.ogg"));
        return buf;
    }

    static sf::SoundBuffer cargarSonidoEnemigoDerrota() {
        sf::SoundBuffer buf;
        buf.loadFromFile(rutaArchivo("assets/music/Sonidosjuego/enemigoderrotado.ogg"));
        return buf;
    }

    static sf::SoundBuffer cargarSonidoDerrota() {
        sf::SoundBuffer buf;
        buf.loadFromFile(rutaArchivo("assets/music/Sonidosjuego/derrotado.ogg"));
        return buf;
    }

    static sf::SoundBuffer cargarSonidoVictoria() {
        sf::SoundBuffer buf;
        buf.loadFromFile(rutaArchivo("assets/music/Sonidosjuego/Juegoganado.ogg"));
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
            tex.loadFromFile(rutaArchivo(nombres[nivel - 1]));
        }
        return tex;
    }
};
