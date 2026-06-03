#ifndef ENTIDADES_HPP
#define ENTIDADES_HPP

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
        sprite.setScale(0.08f, 0.08f); // Ajustar tamaño según tus assets
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
        sprite.setScale(0.1f, 0.1f);
    }

    void actualizar(float dt) {
        sprite.move(0.f, velocidad * dt); // Caen en línea recta
    }
};

class PowerUp {
public:
    sf::Sprite sprite;
    PowerUpType tipo;

    PowerUp(float x, float y, PowerUpType t, const sf::Texture& tex) : tipo(t) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.12f, 0.12f);
    }

    void actualizar(float dt) {
        sprite.move(0.f, 150.f * dt); // Caen lentamente flotando
    }
};

#endif