#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Neighbor.hpp"

/**
 * Structure représentant une cellule de carte
 * Chaque champ est stocké comme un short signé (-32768 à 32767)
 */
struct MapCell
{
    std::int16_t cellNumber;             // Numéro de la cellule
    std::int16_t speed;                  // Vitesse
    std::int16_t mapChangeData;          // Données de changement de carte
    std::int16_t moveZone;               // Zone de mouvement
    std::int16_t linkedZone;             // Zone liée
    std::int16_t mov;                    // Mouvement
    std::int16_t los;                    // Line of sight
    std::int16_t nonWalkableDuringFight; // Non-marchable pendant combat
    std::int16_t nonWalkableDuringRP;    // Non-marchable pendant RP
    std::int16_t farmCell;               // Cellule de ferme
    std::int16_t visible;                // Visible
    std::int16_t havenbagCell;           // Cellule de havenbag
    std::int16_t floor;                  // Sol
    std::int16_t red;                    // Rouge
    std::int16_t blue;                   // Bleu
    std::int16_t arrow;                  // Flèche

    // Constructeur par défaut
    MapCell() = default;

    // Méthode pour afficher les informations de la cellule
    void print(int cellIndex) const;

    // Opérateur de comparaison pour les tests
    bool operator==(const MapCell &other) const;

    // Méthodes utilitaires
    bool isWalkable() const
    {
        return mov == 1;
    }
    bool isFarm() const
    {
        return farmCell == 1;
    }
    bool isVisible() const
    {
        return visible == 1;
    }

    std::string getWalkableStatus() const
    {
        return isWalkable() ? "marchable" : "non-marchable";
    }
};