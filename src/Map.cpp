#include "Map.hpp"

#include <string>
#include <unordered_set>

#include "Neighbor.hpp"
#include "errors/DfbException.hpp"

// Implémentation de la classe Map
Map::Map(const std::vector<MapCell> &cells, int width) : cells_(cells), width_(width)
{
    if (width_ <= 0)
    {
        width_ = DEFAULT_WIDTH;
    }
    height_ = static_cast<int>(std::ceil(static_cast<double>(cells_.size()) / width_));
    buildCellIndex();
}

void Map::buildCellIndex()
{
    cellNumberToIndex_.clear();
    for (size_t i = 0; i < cells_.size(); ++i)
    {
        cellNumberToIndex_[cells_[i].cellNumber] = i;
    }
}

const MapCell *Map::getCellByNumber(int cellNumber) const
{
    auto it = cellNumberToIndex_.find(cellNumber);
    if (it != cellNumberToIndex_.end())
    {
        return &cells_[it->second];
    }
    return nullptr;
}

MapCell *Map::getCellByNumber(int cellNumber)
{
    auto it = cellNumberToIndex_.find(cellNumber);
    if (it != cellNumberToIndex_.end())
    {
        return &cells_[it->second];
    }
    return nullptr;
}

std::pair<int, int> Map::getCellCoordinates(int cellId) const
{
    int x = cellId % width_;
    int y = cellId / width_;
    return {x, y};
}

int Map::getCellNumber(int x, int y) const
{
    return y * width_ + x;
}

bool Map::cellExists(int cellNumber) const
{
    return cellNumberToIndex_.find(cellNumber) != cellNumberToIndex_.end();
}

std::vector<Neighbor> Map::getCellNeighbors(int cellId) const
{
    auto [x, y] = getCellCoordinates(cellId);
    std::vector<Neighbor> neighbors;

    // Utilise les offsets exacts selon si la ligne est paire ou impaire
    if (y % 2 == 0)
    { // Ligne paire
        neighbors = {
            {cellId + 1, Direction::EAST},                // Est
            {cellId + width_, Direction::SOUTH_EAST},     // Sud-Est
            {cellId + width_ * 2, Direction::SOUTH},      // Sud
            {cellId + width_ - 1, Direction::SOUTH_WEST}, // Sud-Ouest
            {cellId - 1, Direction::WEST},                // Ouest
            {cellId - width_ - 1, Direction::NORTH_WEST}, // Nord-Ouest
            {cellId - width_ * 2, Direction::NORTH},      // Nord
            {cellId - width_, Direction::NORTH_EST},      // Nord-Est
        };
    }
    else
    { // Ligne impaire
        neighbors = {
            {cellId + 1, Direction::EAST},                // Est
            {cellId + width_ + 1, Direction::SOUTH_EAST}, // Sud-Est
            {cellId + width_ * 2, Direction::SOUTH},      // Sud
            {cellId + width_, Direction::SOUTH_WEST},     // Sud-Ouest
            {cellId - 1, Direction::WEST},                // Ouest
            {cellId - width_, Direction::NORTH_WEST},     // Nord-Ouest
            {cellId - width_ * 2, Direction::NORTH},      // Nord
            {cellId - width_ + 1, Direction::NORTH_EST},  // Nord-Est
        };
    }

    std::vector<Neighbor> validNeighbors;

    for (const auto &neighbor : neighbors)
    {
        // Vérifier si le voisin est dans les limites de la carte
        if (neighbor.cellId >= 0 && neighbor.cellId < width_ * height_)
        {
            if (isValidNeighbor(neighbor.cellId, x, y, neighbor.direction))
            {
                validNeighbors.push_back(neighbor);
            }
        }
    }

    return validNeighbors;
}

bool Map::isValidNeighbor(int neighborId, int originalX, int originalY, Direction direction) const
{
    // Pour les voisins Nord et Sud qui sont à 2 lignes de distance
    if (direction == Direction::NORTH || direction == Direction::SOUTH)
    {
        if (direction == Direction::NORTH && originalY >= 2)
        { // Pour le Nord
            int ny = originalY - 2;
            int nx = originalX;
            return getCellNumber(nx, ny) == neighborId;
        }
        else if (direction == Direction::SOUTH && originalY + 2 < height_)
        { // Pour le Sud
            int ny = originalY + 2;
            int nx = originalX;
            return getCellNumber(nx, ny) == neighborId;
        }
        return false;
    }
    else
    {
        // Pour les autres directions (diagonales et horizontales)
        auto [nx, ny] = getCellCoordinates(neighborId);
        // Vérifier que le déplacement est cohérent
        int dx = std::abs(nx - originalX);
        int dy = std::abs(ny - originalY);
        return (dx <= 1 && dy <= 1) || (dx == 0 && dy == 2);
    }
}

std::vector<std::pair<Neighbor, const MapCell *>> Map::getNeighborCells(int cellNumber,
                                                                        bool includeDiagonal) const
{
    if (!cellExists(cellNumber))
    {
        throw DfbException("Cell doesn't exist");
    }

    auto neighbors = getCellNeighbors(cellNumber);

    // Filtrer les diagonales si nécessaire
    if (!includeDiagonal)
    {
        std::unordered_set<Direction> nonDiagonalDirections = {
            Direction::SOUTH_EAST, Direction::SOUTH_WEST, Direction::NORTH_WEST,
            Direction::NORTH_EST};
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                                       [&nonDiagonalDirections](const Neighbor &n)
                                       {
                                           return nonDiagonalDirections.find(n.direction) ==
                                                  nonDiagonalDirections.end();
                                       }),
                        neighbors.end());
    }

    std::vector<std::pair<Neighbor, const MapCell *>> neighborCells;

    for (const auto &neighbor : neighbors)
    {
        const MapCell *neighborCell = getCellByNumber(neighbor.cellId);
        neighborCells.emplace_back(neighbor, neighborCell);
    }

    return neighborCells;
}

void Map::printNeighbors(int cellNumber, bool includeDiagonal) const
{
    std::cout << "Recherche des voisins de la cellule " << cellNumber
              << " (diagonal: " << (includeDiagonal ? "true" : "false") << ")" << std::endl;

    auto neighborCells = getNeighborCells(cellNumber, includeDiagonal);

    std::cout << "Voisins trouvés (" << neighborCells.size() << "):" << std::endl;

    for (const auto &[neighbor, cell] : neighborCells)
    {
        if (cell != nullptr)
        {
            std::string farmStatus = cell->isFarm() ? ", ferme" : "";
            std::cout << "  " << neighbor.direction << ": Cellule " << neighbor.cellId << " ("
                      << cell->getWalkableStatus() << farmStatus << ")" << std::endl;
        }
        else
        {
            std::cout << "  " << neighbor.direction << ": Cellule " << neighbor.cellId
                      << " (données non trouvées)" << std::endl;
        }
    }
}

bool Map::hasFourAdjacentCellsFree(int cellNumber, std::set<int> occupiedCells) const
{
    // Obtenir les voisins de la cellule (seulement les directions cardinales : N, S, E, W)
    auto neighbors = getCellNeighbors(cellNumber);

    // Filtrer pour ne garder que les directions cardinales (non-diagonales)
    std::unordered_set<Direction> cardinalDirections = {
        Direction::SOUTH_EAST, Direction::SOUTH_WEST, Direction::NORTH_WEST, Direction::NORTH_EST};
    std::vector<Neighbor> cardinalNeighbors;

    for (const auto &neighbor : neighbors)
    {
        if (cardinalDirections.find(neighbor.direction) != cardinalDirections.end())
        {
            cardinalNeighbors.push_back(neighbor);
        }
    }

    // Vérifier si on a exactement 4 voisins cardinaux
    if (cardinalNeighbors.size() != 4)
    {
        return false;
    }

    // Vérifier que tous les voisins cardinaux sont marchables
    for (const auto &neighbor : cardinalNeighbors)
    {
        const MapCell *neighborCell = getCellByNumber(neighbor.cellId);
        if (neighborCell == nullptr || !neighborCell->isWalkable())
        {
            return false; // Voisin non trouvé ou non marchable
        }
        // Check if the cell is occupied at runtime
        if (occupiedCells.find(neighborCell->cellNumber) != occupiedCells.end())
        {
            return false;
        }
    }

    return true; // Tous les 4 voisins cardinaux sont marchables
}

bool Map::hasFourAdjacentCellsFree(int cellNumber) const
{
    return this->hasFourAdjacentCellsFree(cellNumber, {});
}

std::vector<const MapCell *> Map::getWalkableCells() const
{
    std::vector<const MapCell *> walkableCells;
    for (const auto &cell : cells_)
    {
        if (cell.isWalkable())
        {
            walkableCells.push_back(&cell);
        }
    }
    return walkableCells;
}

std::vector<const MapCell *> Map::getFarmCells() const
{
    std::vector<const MapCell *> farmCells;
    for (const auto &cell : cells_)
    {
        if (cell.isFarm())
        {
            farmCells.push_back(&cell);
        }
    }
    return farmCells;
}