#pragma once
#include <SFML/Graphics.hpp>
#include "entidades.hpp"
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
    bool derribado;
    int frameW;
    int frameH;
    int columnasAnimacion;
    int frameActual;
    float tiempoAnimacion;
    float tiempoDerribado;
    bool moviendo;
    bool mirandoDerecha;

    Personaje() {
        velocidad = 400.f;
        vidas = 3;
        x = 400.f; y = 520.f;
        velocidadY = 0.f;
        enElSuelo = true;
        invencible = false;
        congelado = false;
        disparoTriple = false;
        derribado = false;
        frameW = 1;
        frameH = 1;
        columnasAnimacion = 1;
        frameActual = 0;
        tiempoAnimacion = 0.f;
        tiempoDerribado = 0.f;
        moviendo = false;
        mirandoDerecha = true;
    }

    void configurarSprite(int columnasForzadas = 0) {
        sf::Vector2u size = textura.getSize();
        frameW = static_cast<int>(size.x);
        frameH = static_cast<int>(size.y);
        columnasAnimacion = 1;

        if (columnasForzadas > 0) {
            columnasAnimacion = columnasForzadas;
            frameW = static_cast<int>(size.x / columnasAnimacion);
            frameH = static_cast<int>(size.y);
        } else if (size.x > size.y * 2) {
            columnasAnimacion = 4;
            frameW = static_cast<int>(size.x / 4);
        } else if (size.y > 500 && size.y > size.x * 1.2f) {
            columnasAnimacion = 2;
            frameH = static_cast<int>(size.y / 2);
        } else if (size.x > 700 && size.y > 700) {
            columnasAnimacion = 2;
            frameW = static_cast<int>(size.x / 2);
            frameH = static_cast<int>(size.y / 2);
        }

        frameActual = 0;
        tiempoAnimacion = 0.f;
        sprite.setTexture(textura);
        sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
        sprite.setOrigin(frameW / 2.f, frameH / 2.f);
        sprite.setPosition(x, y);

        float escala = 95.f / static_cast<float>(frameH);
        sprite.setScale(escala, escala);
    }

    void cargarTextura(const std::string& path) {
        if (textura.loadFromFile(path)) {
            configurarSprite();
        }
    }

    void mover(float dirX, float dt) {
        if (congelado || derribado) return;
        moviendo = true;
        mirandoDerecha = dirX >= 0.f;
        x += dirX * velocidad * dt;
        if (x < 40.f) x = 40.f;
        if (x > 760.f) x = 760.f;
        sprite.setPosition(x, y);
    }

    void saltar() {
        if (enElSuelo && !congelado && !derribado) {
            velocidadY = -650.f;
            enElSuelo = false;
        }
    }

    bool estaDerribado() const {
        return derribado;
    }

    void derribar(float direccionGolpe) {
        derribado = true;
        congelado = false;
        tiempoDerribado = 0.85f;
        velocidadY = -260.f;
        enElSuelo = false;
        mirandoDerecha = direccionGolpe < 0.f;
        sprite.setRotation(mirandoDerecha ? -64.f : 64.f);
    }

    void actualizarAnimacion(float dt) {
        if (derribado) {
            frameActual = 0;
            sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
            float escala = 95.f / static_cast<float>(frameH);
            sprite.setScale(mirandoDerecha ? escala : -escala, escala * 0.82f);
            moviendo = false;
            return;
        }

        bool animar = moviendo || !enElSuelo;
        float velocidadFrame = moviendo ? 0.10f : 0.16f;

        if (animar && columnasAnimacion > 1) {
            tiempoAnimacion += dt;
            if (tiempoAnimacion >= velocidadFrame) {
                tiempoAnimacion = 0.f;
                frameActual = (frameActual + 1) % columnasAnimacion;
            }
        } else {
            frameActual = 0;
            tiempoAnimacion = 0.f;
        }

        sprite.setTextureRect(sf::IntRect(frameActual * frameW, 0, frameW, frameH));
        float escala = 95.f / static_cast<float>(frameH);
        sprite.setScale(mirandoDerecha ? escala : -escala, escala);
        sprite.setRotation(0.f);
        moviendo = false;
    }

    void actualizarFisica(float dt) {
        if (derribado) {
            tiempoDerribado -= dt;
            velocidadY += 1650.f * dt;
            y += velocidadY * dt;

            if (y >= 520.f) {
                y = 520.f;
                velocidadY = 0.f;
                enElSuelo = true;
            }

            if (tiempoDerribado <= 0.f && enElSuelo) {
                derribado = false;
                sprite.setRotation(0.f);
            }

            sprite.setPosition(x, y);
            actualizarAnimacion(dt);
            return;
        }

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
        actualizarAnimacion(dt);
    }

    void lanzarBalon(std::vector<Balon>& balones, const sf::Texture& texBalon) {
        if (congelado || derribado) return;
        
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
