#include "Map.hpp"

#include <string>
#include <unordered_set>

#include "Neighbor.hpp"
#include "errors/DfbException.hpp"
#include <queue>

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

bool Map::isLos(int startCell, int endCell) const {
    return isLos(startCell, endCell, std::set<int>());
}

bool Map::isLos(int startCell, int endCell, std::set<int> occupiedCells) const {
    // Vérifier que les cellules existent
    if (!cellExists(startCell) || !cellExists(endCell)) {
        return false;
    }
    
    // Si c'est la même cellule, il y a LoS
    if (startCell == endCell) {
        return true;
    }
    
    // Obtenir les centres des cellules
    Point2D startCenter = getCellCenter(startCell);
    Point2D endCenter = getCellCenter(endCell);
    
    // Calculer la distance entre start et end pour filtrer les obstacles
    double distStartEnd = std::sqrt((endCenter.x - startCenter.x) * (endCenter.x - startCenter.x) + 
                                   (endCenter.y - startCenter.y) * (endCenter.y - startCenter.y));
    
    // Obtenir toutes les cellules potentiellement sur le chemin
    std::vector<int> cellsOnPath = getCellsOnLine(startCenter, endCenter);

    for (int cellId : cellsOnPath) {
        std::cout << "Cellule sur le chemin: " << cellId << std::endl;
    }
    if (cellsOnPath.empty()) {
        std::cout << "Aucune cellule sur le chemin" << std::endl;
        return true; // Pas d'obstacles si aucune cellule sur le chemin
    }
    
    // Collecter tous les obstacles (cellules avec los = 0 ou occupées)
    std::vector<int> obstacles;
    for (int cellId : cellsOnPath) {
        if (cellId == startCell || cellId == endCell) {
            continue; // Ignorer les cellules de départ et d'arrivée
        }
        
        const MapCell* cell = getCellByNumber(cellId);
        if (cell && (cell->los == 0 || occupiedCells.find(cellId) != occupiedCells.end())) {
            // Vérifier que l'obstacle est entre start et end
            Point2D obstacleCenter = getCellCenter(cellId);
            double distStartObstacle = std::sqrt((obstacleCenter.x - startCenter.x) * (obstacleCenter.x - startCenter.x) + 
                                               (obstacleCenter.y - startCenter.y) * (obstacleCenter.y - startCenter.y));
            
            // L'obstacle doit être plus proche que la cible
            if (distStartObstacle < distStartEnd) {
                obstacles.push_back(cellId);
            }
        }
    }
    
    // S'il n'y a pas d'obstacles, il y a LoS
    if (obstacles.empty()) {
        std::cout << "Pas d'obstacles, LoS degagee" << std::endl;
        return true;
    }
    
    // Calculer les secteurs d'ombre pour chaque obstacle
    std::vector<std::pair<ShadowSector, int>> shadowSectorsWithSource;
    
    for (int obstacleId : obstacles) {
        // Obtenir les 4 coins de l'obstacle
        std::vector<Point2D> corners = getCellCorners(obstacleId);
        
        // Calculer les angles depuis startCenter vers chaque coin
        std::vector<double> angles;
        for (const Point2D& corner : corners) {
            angles.push_back(getAngle(startCenter, corner));
        }
        
        // Trouver l'angle minimum et maximum qui forment le plus grand secteur
        double minAngle = angles[0];
        double maxAngle = angles[0];
        double maxSectorSize = 0;
        
        // Tester toutes les paires d'angles pour trouver le secteur le plus large
        for (size_t i = 0; i < angles.size(); ++i) {
            for (size_t j = i + 1; j < angles.size(); ++j) {
                double a1 = angles[i];
                double a2 = angles[j];
                
                // Calculer la taille du secteur dans les deux sens
                double diff1 = normalizeAngle(a2 - a1);
                double diff2 = normalizeAngle(a1 - a2);
                
                if (diff1 < diff2) {
                    if (diff1 > maxSectorSize) {
                        maxSectorSize = diff1;
                        minAngle = a1;
                        maxAngle = a2;
                    }
                } else {
                    if (diff2 > maxSectorSize) {
                        maxSectorSize = diff2;
                        minAngle = a2;
                        maxAngle = a1;
                    }
                }
            }
        }
        
        // Créer le secteur d'ombre pour cet obstacle avec son ID
        shadowSectorsWithSource.push_back({ShadowSector(minAngle, maxAngle), obstacleId});
    }
    
    // Vérifier si le centre de la cellule cible est dans l'ombre
    double targetAngle = getAngle(startCenter, endCenter);
    std::cout << "Angle cible: " << targetAngle << std::endl;
    
    // Vérifier dans quel secteur d'ombre se trouve la cible
    for (const auto& [shadow, obstacleId] : shadowSectorsWithSource) {
        double minAngle = normalizeAngle(shadow.angleMin);
        double maxAngle = normalizeAngle(shadow.angleMax);
        double checkAngle = normalizeAngle(targetAngle);
        
        bool inShadow = false;
        
        // Calculer la différence d'angle dans le bon sens
        double angleDiff = normalizeAngle(maxAngle - minAngle);
        double targetDiff = normalizeAngle(checkAngle - minAngle);
        std::cout << "AngleDiff: " << angleDiff << " for " << obstacleId << std::endl;
        if (targetDiff <= angleDiff) {
            inShadow = true;
        }
        
        if (inShadow) {
            // std::cout << "LoS bloquée par la cellule " << obstacleId 
            //          << " (ombre projetée de " << startCell << " vers " << endCell << ")" << std::endl;
            return false;
        }
    }
    
    return true;
}

Map::Point2D Map::getCellCenter(int cellId) const {
    auto [x, y] = getCellCoordinates(cellId);
    
    // Facteur d'échelle (comme dans le code Python)
    const double scaleX = 1.5;
    const double scaleY = 1.125;
    
    bool oddCell = (y % 2 == 1);
    
    double posX = x * scaleX + oddCell * 0.5 * scaleX;
    double posY = y * scaleY * 0.5;
    
    return Point2D(posX, posY);
}

std::vector<Map::Point2D> Map::getCellCorners(int cellId) const {
    Point2D center = getCellCenter(cellId);
    
    // Taille du losange
    const double sizeX = 0.5 * 1.5;  // scale_x = 1.5
    const double sizeY = 0.5 * 1.125; // scale_y = 1.125
    
    std::vector<Point2D> corners;
    corners.push_back(Point2D(center.x, center.y - sizeY));      // Haut
    corners.push_back(Point2D(center.x + sizeX, center.y));      // Droite
    corners.push_back(Point2D(center.x, center.y + sizeY));      // Bas
    corners.push_back(Point2D(center.x - sizeX, center.y));      // Gauche
    
    return corners;
}

double Map::getAngle(const Point2D& origin, const Point2D& target) const {
    double dx = target.x - origin.x;
    double dy = target.y - origin.y;
    return normalizeAngle(std::atan2(dy, dx));
}

double Map::normalizeAngle(double angle) const {
    while (angle < 0) angle += 2 * M_PI;
    while (angle >= 2 * M_PI) angle -= 2 * M_PI;
    return angle;
}

bool Map::isAngleInShadow(double angle, const std::vector<ShadowSector>& shadows) const {
    angle = normalizeAngle(angle);
    
    for (const ShadowSector& shadow : shadows) {
        double minAngle = normalizeAngle(shadow.angleMin);
        double maxAngle = normalizeAngle(shadow.angleMax);
        
        // Cas simple : le secteur ne passe pas par 0
        if (minAngle <= maxAngle) {
            if (angle >= minAngle && angle <= maxAngle) {
                return true;
            }
        }
        // Cas complexe : le secteur passe par 0
        else {
            if (angle >= minAngle || angle <= maxAngle) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<Map::ShadowSector> Map::mergeShadowSectors(std::vector<ShadowSector>& sectors) const {
    if (sectors.size() <= 1) {
        return sectors;
    }
    
    // Trier les secteurs par angle minimum
    std::sort(sectors.begin(), sectors.end(), 
        [this](const ShadowSector& a, const ShadowSector& b) {
            return normalizeAngle(a.angleMin) < normalizeAngle(b.angleMin);
        });
    
    std::vector<ShadowSector> merged;
    merged.push_back(sectors[0]);
    
    for (size_t i = 1; i < sectors.size(); ++i) {
        ShadowSector& current = sectors[i];
        ShadowSector& last = merged.back();
        
        double lastMax = normalizeAngle(last.angleMax);
        double currentMin = normalizeAngle(current.angleMin);
        
        // Vérifier le chevauchement
        bool overlap = false;
        
        // Cas où last ne passe pas par 0
        if (normalizeAngle(last.angleMin) <= lastMax) {
            if (currentMin <= lastMax) {
                overlap = true;
            }
        }
        // Cas où last passe par 0
        else {
            overlap = true; // Dans ce cas, on fusionne toujours
        }
        
        if (overlap) {
            // Fusionner les secteurs
            last.angleMax = std::max(last.angleMax, current.angleMax);
        } else {
            merged.push_back(current);
        }
    }
    
    return merged;
}

std::vector<int> Map::getCellsOnLine(const Point2D& start, const Point2D& end) const {
    std::vector<int> cells;
    std::set<int> uniqueCells; // Pour éviter les doublons
    
    // Utiliser l'algorithme de Bresenham adapté pour la grille en losanges
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double distance = std::sqrt(dx * dx + dy * dy);
    
    // Échantillonner le long de la ligne
    int steps = static_cast<int>(distance * 10); // Plus de précision
    
    for (int i = 0; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        double x = start.x + t * dx;
        double y = start.y + t * dy;
        
        // Convertir la position en cellule
        const double scaleX = 1.5;
        const double scaleY = 1.125;
        
        // Estimation de la ligne
        int estimatedY = static_cast<int>(y / (scaleY * 0.5) + 0.5);
        bool oddRow = (estimatedY % 2 == 1);
        
        int cellX = static_cast<int>((x - oddRow * 0.5 * scaleX) / scaleX + 0.5);
        int cellY = estimatedY;
        
        if (cellX >= 0 && cellX < width_ && cellY >= 0 && cellY < height_) {
            int cellId = getCellNumber(cellX, cellY);
            
            // Vérifier que la cellule est vraiment sur ou très proche de la ligne
            Point2D cellCenter = getCellCenter(cellId);
            
            // Distance point-ligne
            double crossProduct = (end.y - start.y) * (cellCenter.x - start.x) - 
                                 (end.x - start.x) * (cellCenter.y - start.y);
            double distToLine = std::abs(crossProduct) / distance;
            
            // Si la cellule est très proche de la ligne (seuil plus strict)
            if (distToLine < 0.5) {
                uniqueCells.insert(cellId);
            }
        }
    }
    
    // Convertir le set en vector
    cells.assign(uniqueCells.begin(), uniqueCells.end());
    return cells;
}