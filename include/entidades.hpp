#pragma once
#include <SFML/Graphics.hpp>

enum class PowerUpType { NINGUNO, TAQUETES, ESPINILLERAS, GUANTES, BANDA_CAPITAN, TARJETA_AMARILLA, TARJETA_ROJA };

class Balon {
public:
    sf::Sprite sprite;
    float vx, vy;

    Balon(float x, float y, float vx, float vy, const sf::Texture& tex) : vx(vx), vy(vy) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.24f, 0.24f);
    }

    void actualizar(float dt) {
        sprite.move(vx * dt, vy * dt);
    }
};

class AtaqueEnemigo {
public:
    sf::Sprite sprite;
    float velocidad;
    PowerUpType tipoDano; 

    AtaqueEnemigo(float x, float y, float vel, const sf::Texture& tex, PowerUpType tipo = PowerUpType::NINGUNO) 
        : velocidad(vel), tipoDano(tipo) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.22f, 0.22f);
    }

    void actualizar(float dt) {
        sprite.move(0.f, velocidad * dt);
    }
};

class PowerUp {
public:
    sf::Sprite sprite;
    PowerUpType tipo;
    float velocidadY;

    PowerUp(float x, float y, PowerUpType t, const sf::Texture& tex, float velY = 0.f) : tipo(t), velocidadY(velY) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.28f, 0.28f);
    }

    void actualizar(float dt) {
        sprite.move(0.f, velocidadY * dt);
    }
};
