#pragma once
#include <SFML/Graphics.hpp>
#include "entidades.hpp"
#include "recursos.hpp"
#include <iostream>

class InterfazUsuario {
public:
    sf::Font fuente;

    InterfazUsuario() {
        if (!fuente.loadFromFile(Recursos::rutaArchivo("assets/fuentes/Arial.ttf"))) {
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

    void dibujarCorazonHUD(sf::RenderWindow& window, float x, float y, bool lleno) {
        sf::Color color = lleno ? sf::Color(255, 65, 95) : sf::Color(75, 60, 68);
        sf::Color borde = lleno ? sf::Color(255, 210, 220) : sf::Color(125, 100, 110);

        sf::CircleShape izquierda(5.f);
        izquierda.setOrigin(5.f, 5.f);
        izquierda.setPosition(x + 5.f, y + 5.f);
        izquierda.setFillColor(color);
        izquierda.setOutlineColor(borde);
        izquierda.setOutlineThickness(1.f);
        window.draw(izquierda);

        sf::CircleShape derecha(5.f);
        derecha.setOrigin(5.f, 5.f);
        derecha.setPosition(x + 13.f, y + 5.f);
        derecha.setFillColor(color);
        derecha.setOutlineColor(borde);
        derecha.setOutlineThickness(1.f);
        window.draw(derecha);

        sf::ConvexShape punta;
        punta.setPointCount(3);
        punta.setPoint(0, sf::Vector2f(x + 1.f, y + 7.f));
        punta.setPoint(1, sf::Vector2f(x + 17.f, y + 7.f));
        punta.setPoint(2, sf::Vector2f(x + 9.f, y + 20.f));
        punta.setFillColor(color);
        punta.setOutlineColor(borde);
        punta.setOutlineThickness(1.f);
        window.draw(punta);
    }

    void dibujarHUD(sf::RenderWindow& window, int puntos, int nivel, int vidas, int vidaEnemigo, PowerUpType power, float tRestante) {
        sf::RectangleShape banner(sf::Vector2f(800.f, 50.f));
        banner.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(banner);

        dibujarTexto(window, "PUNTOS: " + std::to_string(puntos), 20.f, 10.f, 20, sf::Color::Yellow);
        dibujarTexto(window, "NIVEL: " + std::to_string(nivel), 180.f, 10.f, 20, sf::Color::Cyan);
        dibujarTexto(window, "VIDAS:", 310.f, 10.f, 20, sf::Color(255, 120, 140));

        int vidasVisibles = vidas;
        if (vidasVisibles < 0) vidasVisibles = 0;
        if (vidasVisibles > 3) vidasVisibles = 3;
        for (int i = 0; i < 3; ++i) {
            dibujarCorazonHUD(window, 386.f + i * 24.f, 14.f, i < vidasVisibles);
        }

        dibujarTexto(window, "RIVAL HP: " + std::to_string(vidaEnemigo), 485.f, 10.f, 20, sf::Color::Green);

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
