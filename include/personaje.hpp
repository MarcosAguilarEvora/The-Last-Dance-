#pragma once
#include <SFML/Graphics.hpp>
#include "Entidades.hpp"
#include <vector>

class Personaje {
public:
    sf::Sprite sprite;
    sf::Texture textura;
    float velocidad;
    int vidas;
    float x, y;
    float velocidadY;
    bool enElSuelo;
    bool invencible;
    bool congelado;
    bool disparoTriple;

    Personaje() {
        velocidad = 400.f;
        vidas = 3;
        x = 400.f; y = 520.f;
        velocidadY = 0.f;
        enElSuelo = true;
        invencible = false;
        congelado = false;
        disparoTriple = false;
    }

    void cargarTextura(const std::string& path) {
        if (textura.loadFromFile(path)) {
            sprite.setTexture(textura);
            sprite.setOrigin(textura.getSize().x / 2.f, textura.getSize().y / 2.f);
            sprite.setPosition(x, y);
            sprite.setScale(0.2f, 0.2f);
        }
    }

    void mover(float dirX, float dt) {
        if (congelado) return;
        x += dirX * velocidad * dt;
        if (x < 40.f) x = 40.f;
        if (x > 760.f) x = 760.f;
        sprite.setPosition(x, y);
    }

    void saltar() {
        if (enElSuelo && !congelado) {
            velocidadY = -650.f;
            enElSuelo = false;
        }
    }

    void actualizarFisica(float dt) {
        if (!enElSuelo) {
            velocidadY += 1800.f * dt;
            y += velocidadY * dt;
            if (y >= 520.f) {
                y = 520.f;
                velocidadY = 0.f;
                enElSuelo = true;
            }
            sprite.setPosition(x, y);
        }
    }

    void lanzarBalon(std::vector<Balon>& balones, const sf::Texture& texBalon) {
        if (congelado) return;
        
        if (disparoTriple) {
            balones.push_back(Balon(x, y - 30.f, 0.f, -700.f, texBalon));      
            balones.push_back(Balon(x, y - 30.f, -200.f, -650.f, texBalon));  
            balones.push_back(Balon(x, y - 30.f, 200.f, -650.f, texBalon));   
        } else {
            balones.push_back(Balon(x, y - 30.f, 0.f, -700.f, texBalon));      
        }
    }

    void aplicarPowerUp(PowerUpType tipo) {
        desactivarPowerUps();
        switch (tipo) {
            case PowerUpType::TAQUETES: velocidad = 650.f; break;
            case PowerUpType::ESPINILLERAS: invencible = true; break;
            case PowerUpType::GUANTES: invencible = true; break;
            case PowerUpType::BANDA_CAPITAN: velocidad = 600.f; invencible = true; disparoTriple = true; break;
            case PowerUpType::TARJETA_AMARILLA: congelado = true; break;
            case PowerUpType::TARJETA_ROJA: vidas = 0; break;
            default: break;
        }
    }

    void desactivarPowerUps() {
        velocidad = 400.f;
        invencible = false;
        congelado = false;
        disparoTriple = false;
    }
};