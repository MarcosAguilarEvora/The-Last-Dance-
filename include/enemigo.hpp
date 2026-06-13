#pragma once
#include <SFML/Graphics.hpp>
#include "entidades.hpp"
#include <vector>
#include <cmath>

class Enemigo {
public:
    sf::Sprite sprite;
    sf::Texture textura;
    std::string nombre;
    int vida;
    float velocidad;
    float x, y;
    int direccion;
    float tempAtaque;
    float intervaloAtaque;
    int nivel;

    Enemigo(std::string nom, int nvl, const sf::Texture& tex) {
        nombre = nom;
        nivel = nvl;
        vida = 18 + (nvl * 4);
        velocidad = 155.f + (nvl * 55.f); 
        x = 400.f; y = 100.f;
        direccion = 1;
        tempAtaque = 0.f;
        intervaloAtaque = calcularIntervaloAtaque();
        
        textura = tex;
        sf::Vector2u size = textura.getSize();
        int frameW = static_cast<int>(size.x);
        int frameH = static_cast<int>(size.y);

        if (size.x > 0 && size.y > 0) {
            frameW = static_cast<int>(size.x / 3);
            frameH = static_cast<int>(size.y / 2);
        }

        sprite.setTexture(textura);
        sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
        sprite.setOrigin(frameW / 2.f, frameH / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.32f, 0.32f);
    }

    float calcularIntervaloAtaque() const {
        float intervalo = 1.45f - ((nivel - 1) * 0.20f);
        return intervalo < 0.65f ? 0.65f : intervalo;
    }

    float calcularVelocidadProyectil() const {
        return 360.f + (nivel * 95.f);
    }

    int maxAtaquesActivos() const {
        return 1 + (nivel / 3);
    }

    void actualizar(float dt) {
        x += direccion * velocidad * dt;
        if (x > 730.f) { x = 730.f; direccion = -1; }
        if (x < 70.f) { x = 70.f; direccion = 1; }
        
        float flotarY = 100.f + std::sin(x * (0.045f + nivel * 0.004f)) * (12.f + nivel * 3.f);
        float escala = 0.32f + std::sin(x * 0.03f) * 0.015f;
        sprite.setPosition(x, flotarY);
        sprite.setScale(escala, escala);
    }

    void atacar(float dt, std::vector<AtaqueEnemigo>& ataques, const sf::Texture& texProyectil) {
        if (ataques.size() >= static_cast<size_t>(maxAtaquesActivos())) return;

        tempAtaque += dt;
        if (tempAtaque >= intervaloAtaque) {
            tempAtaque = 0.f;
            ataques.push_back(AtaqueEnemigo(x, sprite.getPosition().y + 30.f, calcularVelocidadProyectil(), texProyectil));
        }
    }
};
