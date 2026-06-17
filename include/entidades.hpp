#pragma once
#include <SFML/Graphics.hpp>

enum class PowerUpType { NINGUNO, TAQUETES, ESPINILLERAS, GUANTES, BANDA_CAPITAN, TARJETA_AMARILLA, TARJETA_ROJA, VIDA_EXTRA };

class Balon {
public:
    sf::Sprite sprite;
    float vx, vy;

    Balon(float x, float y, float vx, float vy, const sf::Texture& tex) : vx(vx), vy(vy) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.16f, 0.16f);
    }

    void actualizar(float dt) {
        sprite.move(vx * dt, vy * dt);
    }
};

class AtaqueEnemigo {
public:
    sf::Sprite sprite;
    float vx;
    float velocidad;
    PowerUpType tipoDano; 

    AtaqueEnemigo(float x, float y, float vel, const sf::Texture& tex, PowerUpType tipo = PowerUpType::NINGUNO, float vx = 0.f) 
        : vx(vx), velocidad(vel), tipoDano(tipo) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.16f, 0.16f);
    }

    void actualizar(float dt) {
        sprite.move(vx * dt, velocidad * dt);
    }
};

class PowerUp {
public:
    sf::Sprite sprite;
    PowerUpType tipo;
    float velocidadY;
    float tiempoVida;
    float duracionInicial;

    PowerUp(float x, float y, PowerUpType t, const sf::Texture& tex, float velY = 0.f, float duracion = 5.f) : tipo(t), velocidadY(velY), tiempoVida(duracion), duracionInicial(duracion) {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setScale(0.20f, 0.20f);
    }

    void actualizar(float dt) {
        tiempoVida -= dt;
        sprite.move(0.f, velocidadY * dt);
    }

    bool expiro() const {
        return tiempoVida <= 0.f;
    }

    float vidaNormalizada() const {
        if (duracionInicial <= 0.f) return 0.f;
        float ratio = tiempoVida / duracionInicial;
        if (ratio < 0.f) return 0.f;
        if (ratio > 1.f) return 1.f;
        return ratio;
    }
};
