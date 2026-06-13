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
        vida = 20;
        velocidad = 150.f + (nvl * 45.f); 
        x = 400.f; y = 100.f;
        direccion = 1;
        tempAtaque = 0.f;
        intervaloAtaque = 1.5f;
        
        textura = tex;
        sprite.setTexture(textura);
        sprite.setOrigin(textura.getSize().x / 2.f, textura.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.25f, 0.25f);
    }

    void actualizar(float dt) {
        x += direccion * velocidad * dt;
        if (x > 730.f) { x = 730.f; direccion = -1; }
        if (x < 70.f) { x = 70.f; direccion = 1; }
        
        float flotarY = 100.f + std::sin(x * (0.045f + nivel * 0.004f)) * (12.f + nivel * 3.f);
        float escala = 0.25f + std::sin(x * 0.03f) * 0.015f;
        sprite.setPosition(x, flotarY);
        sprite.setScale(escala, escala);
    }

    void atacar(float dt, std::vector<AtaqueEnemigo>& ataques, const sf::Texture& texProyectil) {
        if (!ataques.empty()) return;

        tempAtaque += dt;
        if (tempAtaque >= intervaloAtaque) {
            tempAtaque = 0.f;
            ataques.push_back(AtaqueEnemigo(x, sprite.getPosition().y + 30.f, 320.f + (nivel * 75.f), texProyectil));
        }
    }
};
