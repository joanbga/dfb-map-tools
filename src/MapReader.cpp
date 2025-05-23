#include "MapReader.hpp"

#include <fstream>

// Implémentation de MapReader
std::vector<MapCell> MapReader::readMapFromBinary(const std::string &filename)
{
    std::vector<MapCell> cells;
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Erreur: impossible d'ouvrir le fichier " << filename << std::endl;
        return cells; // Retourne un vecteur vide
    }

    cells.reserve(CELLS_PER_MAP);
    MapCell cell;

    while (cells.size() < CELLS_PER_MAP &&
           file.read(reinterpret_cast<char *>(&cell), sizeof(MapCell)))
    {
        cells.push_back(cell);
    }

    if (file.bad())
    {
        std::cerr << "Erreur lors de la lecture du fichier " << filename << std::endl;
        cells.clear();
    }

    file.close();
    return cells;
}

int MapReader::readMapFromBinary(const std::string &filename, MapCell *cells, size_t maxCells)
{
    if (!cells || maxCells == 0)
    {
        return -1;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Erreur: impossible d'ouvrir le fichier " << filename << std::endl;
        return -1;
    }

    size_t cellsRead = 0;
    while (cellsRead < maxCells && cellsRead < CELLS_PER_MAP &&
           file.read(reinterpret_cast<char *>(&cells[cellsRead]), sizeof(MapCell)))
    {
        cellsRead++;
    }

    if (file.bad() && cellsRead == 0)
    {
        std::cerr << "Erreur lors de la lecture du fichier " << filename << std::endl;
        return -1;
    }

    file.close();
    return static_cast<int>(cellsRead);
}

Map MapReader::readMapObjectFromBinary(const std::string &filename, int width)
{
    auto cells = readMapFromBinary(filename);
    if (cells.empty())
    {
        return Map(); // Retourne une carte vide
    }
    return Map(cells, width);
}