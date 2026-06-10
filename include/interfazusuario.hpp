#pragma once
#include <SFML/Graphics.hpp>
#include "entidades.hpp"
#include <iostream>

class InterfazUsuario {
public:
    sf::Font fuente;

    InterfazUsuario() {
        if (!fuente.loadFromFile("assets/fuentes/Arial.ttf")) {
            fuente.loadFromFile("C:/Windows/Fonts/arial.ttf");
        }
    }

    void dibujarTexto(sf::RenderWindow& window, const std::string& texto, float x, float y, int tam, sf::Color color) {
        sf::Text t;
        t.setFont(fuente);
        t.setString(texto);
        t.setCharacterSize(tam);
        t.setFillColor(color);
        t.setPosition(x, y);
        window.draw(t);
    }

    void dibujarHUD(sf::RenderWindow& window, int puntos, int nivel, int vidas, int vidaEnemigo, PowerUpType power, float tRestante) {
        sf::RectangleShape banner(sf::Vector2f(800.f, 50.f));
        banner.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(banner);

        dibujarTexto(window, "PUNTOS: " + std::to_string(puntos), 20.f, 10.f, 20, sf::Color::Yellow);
        dibujarTexto(window, "NIVEL: " + std::to_string(nivel), 180.f, 10.f, 20, sf::Color::Cyan);
        dibujarTexto(window, "VIDAS: " + std::to_string(vidas), 320.f, 10.f, 20, sf::Color::Red);
        dibujarTexto(window, "RIVAL HP: " + std::to_string(vidaEnemigo), 460.f, 10.f, 20, sf::Color::Green);

        std::string pName = "NINGUNO";
        sf::Color pColor = sf::Color::White;
        if(power == PowerUpType::TAQUETES) { pName = "TAQUETES"; pColor = sf::Color::Magenta; }
        if(power == PowerUpType::ESPINILLERAS) { pName = "ESPINILLERAS"; pColor = sf::Color::Cyan; }
        if(power == PowerUpType::GUANTES) { pName = "GUANTES DE PORTERO"; pColor = sf::Color::Green; }
        if(power == PowerUpType::BANDA_CAPITAN) { pName = "BANDA DE CAPITAN"; pColor = sf::Color::Yellow; }
        if(power == PowerUpType::TARJETA_AMARILLA) { pName = "CONGELADO"; pColor = sf::Color::Red; }

        if (power != PowerUpType::NINGUNO) {
            dibujarTexto(window, "PODER: " + pName + " (" + std::to_string((int)tRestante) + "s)", 20.f, 560.f, 18, pColor);
        }
    }
};