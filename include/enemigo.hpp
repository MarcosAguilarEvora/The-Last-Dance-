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
        velocidad = 150.f + (nvl * 30.f); 
        x = 400.f; y = 100.f;
        direccion = 1;
        tempAtaque = 0.f;
        intervaloAtaque = std::max(0.4f, 1.5f - (nvl * 0.2f)); 
        
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
        
        float flotarY = 100.f + std::sin(x * 0.05f) * 15.f;
        sprite.setPosition(x, flotarY);
    }

    void atacar(float dt, std::vector<AtaqueEnemigo>& ataques, const sf::Texture& texProyectil) {
        tempAtaque += dt;
        if (tempAtaque >= intervaloAtaque) {
            tempAtaque = 0.f;
            ataques.push_back(AtaqueEnemigo(x, sprite.getPosition().y + 30.f, 300.f + (nivel * 40.f), texProyectil));
        }
    }
};