#pragma once

#include <unordered_map>
#include <vector>

#include "MapCell.hpp"
#include "Neighbor.hpp"
#include <set>

/**
 * Classe pour gérer une carte avec ses cellules et leurs relations
 */
class Map
{
public:
    static constexpr int DEFAULT_WIDTH = 14;
    static constexpr size_t CELLS_PER_MAP = 560; // 20x28

private:
    std::vector<MapCell> cells_;
    std::unordered_map<int, size_t> cellNumberToIndex_;
    int width_;
    int height_;

public:
    /**
     * Constructeur avec liste de cellules
     */
    explicit Map(const std::vector<MapCell> &cells, int width = DEFAULT_WIDTH);

    /**
     * Constructeur par défaut
     */
    Map() : width_(DEFAULT_WIDTH), height_(0) {}

    // Getters
    const std::vector<MapCell> &getCells() const
    {
        return cells_;
    }
    int getWidth() const
    {
        return width_;
    }
    int getHeight() const
    {
        return height_;
    }
    size_t getCellCount() const
    {
        return cells_.size();
    }

    /**
     * Obtenir une cellule par son numéro
     */
    const MapCell *getCellByNumber(int cellNumber) const;
    MapCell *getCellByNumber(int cellNumber);

    /**
     * Convertir un ID de cellule en coordonnées (x, y)
     */
    std::pair<int, int> getCellCoordinates(int cellId) const;

    /**
     * Convertir des coordonnées (x, y) en numéro de cellule
     */
    int getCellNumber(int x, int y) const;

    /**
     * Obtenir tous les voisins d'une cellule
     */
    std::vector<Neighbor> getCellNeighbors(int cellId) const;

    /**
     * Obtenir les cellules voisines avec leurs données
     */
    std::vector<std::pair<Neighbor, const MapCell *>> getNeighborCells(
        int cellNumber, bool includeDiagonal = true) const;

    /**
     * Vérifier si une cellule a ses 4 voisins cardinaux libres (marchables)
     */
    bool hasFourAdjacentCellsFree(int cellNumber) const;
    bool hasFourAdjacentCellsFree(int cellNumber, std::set<int> occupiedCells) const;

    /**
     * Afficher les informations des voisins d'une cellule
     */
    void printNeighbors(int cellNumber, bool includeDiagonal = true) const;

    /**
     * Vérifier si une cellule existe
     */
    bool cellExists(int cellNumber) const;

    /**
     * Obtenir toutes les cellules marchables
     */
    std::vector<const MapCell *> getWalkableCells() const;

    /**
     * Obtenir toutes les cellules de ferme
     */
    std::vector<const MapCell *> getFarmCells() const;

private:
    /**
     * Construire l'index cellNumber -> index dans le vecteur
     */
    void buildCellIndex();

    /**
     * Vérifier si un voisin est valide
     */
    bool isValidNeighbor(int neighborId, int originalX, int originalY, Direction direction) const;
};