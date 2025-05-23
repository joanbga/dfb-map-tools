#pragma once

#include <unordered_map>
#include <vector>

#include "MapCell.hpp"
#include "Neighbor.hpp"
#include <set>

#define M_PI       3.14159265358979323846

// Structure pour les coordonnées hexagonales cubiques
    struct HexCube
    {
        double x, y, z;
    };

    struct AxialCoord
    {
        double q, r;
    };

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
     * Vérifie si il y a une ligne de vue (Line of Sight) entre deux cellules
     * @param startCell Cellule de départ
     * @param endCell Cellule d'arrivée  
     * @param occupiedCells Set des cellules occupées qui bloquent la LoS
     * @return true si la ligne de vue est dégagée, false sinon
     */
    bool isLos(int startCell, int endCell, std::set<int> occupiedCells) const;

    /**
     * Surcharge sans paramètre occupiedCells (utilise un set vide)
     */
    bool isLos(int startCell, int endCell) const;

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

    private:
    /**
     * Structure pour représenter un point 2D
     */
    struct Point2D {
        double x, y;
        Point2D(double x = 0, double y = 0) : x(x), y(y) {}
    };

    /**
     * Structure pour représenter un secteur d'ombre
     */
    struct ShadowSector {
        double angleMin;
        double angleMax;
        ShadowSector(double min, double max) : angleMin(min), angleMax(max) {}
    };

    /**
     * Obtenir la position 2D du centre d'une cellule
     */
    Point2D getCellCenter(int cellId) const;

    /**
     * Obtenir les 4 coins d'une cellule (losange)
     */
    std::vector<Point2D> getCellCorners(int cellId) const;

    /**
     * Calculer l'angle entre deux points par rapport à l'origine
     */
    double getAngle(const Point2D& origin, const Point2D& target) const;

    /**
     * Normaliser un angle entre 0 et 2*PI
     */
    double normalizeAngle(double angle) const;

    /**
     * Vérifier si un angle est dans un secteur d'ombre
     */
    bool isAngleInShadow(double angle, const std::vector<ShadowSector>& shadows) const;

    /**
     * Fusionner les secteurs d'ombre qui se chevauchent
     */
    std::vector<ShadowSector> mergeShadowSectors(std::vector<ShadowSector>& sectors) const;

    /**
     * Obtenir toutes les cellules sur une ligne entre deux points
     */
    std::vector<int> getCellsOnLine(const Point2D& start, const Point2D& end) const;
};