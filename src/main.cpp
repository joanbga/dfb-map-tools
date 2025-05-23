#include <iostream>
#include <vector>

#include "Commands.hpp"
#include "DfbException.hpp"
#include "Map.hpp"
#include "MapCell.hpp"
#include "MapReader.hpp"
#include <set>

std::set<int> getOccupiedCellsFromArgs(const std::vector<std::string> &args, int startIndex)
{
    std::set<int> occupiedCells;
    for (size_t i = startIndex; i < args.size(); ++i)
    {
        try
        {
            int occupiedCellId = std::stoi(args[i]);
            occupiedCells.insert(occupiedCellId);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Error: '" << args[i] << "' is not a valid cellId" << std::flush;
            return {};
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Error: '" << args[i] << "' too big for cellId" << std::flush;
            return {};
        }
    }
    return occupiedCells;
}

int hasFourAdjacentCellsFree(const std::string &programParam, const std::vector<std::string> &args,
                             const Map &map)
{
    if (args.size() < 1)
    {
        std::cerr << "Usage: hasFourAdjacentCellsFree <cellId> [occupiedCells...]" << std::endl;
        return 1;
    }
    int cellId = std::stoi(args[0]);
    std::set<int> occupiedCells = getOccupiedCellsFromArgs(args, 1);
    if (map.hasFourAdjacentCellsFree(cellId, occupiedCells))
    {
        std::cout << "OK" << std::flush;
        return 0;
    }
    std::cout << "KO" << std::flush;
    return 1;
}

int getNeighbors(const std::string &programParam, const std::vector<std::string> &args,
                 const Map &map)
{
    if (args.size() < 1)
    {
        std::cerr << "Usage: getNeighbors <cellId>" << std::endl;
        return 1;
    }
    int cellId = std::stoi(args[0]);
    try
    {
        for (auto pair : map.getNeighborCells(cellId))
        {
            auto cell = pair.second;
            auto neighbor = pair.first;
            bool isFree = cell->isWalkable();
            std::cout << neighbor.cellId << ": " << neighbor.direction << " "
                      << (isFree ? "Free" : "Not Free") << std::endl;
        }
    }
    catch (const DfbException &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

int isLos(const std::string &programParam, const std::vector<std::string> &args,
                 const Map &map)
{
    if (args.size() < 2)
    {
        std::cerr << "Usage: isLos <startCellId> <endCellId> [occupiedCells...]" << std::endl;
        return 1;
    }
    int startCellId = std::stoi(args[0]);
    int endCellId = std::stoi(args[1]);
    std::set<int> occupiedCells = getOccupiedCellsFromArgs(args, 2);
    if (map.isLos(startCellId, endCellId, occupiedCells))
    {
        std::cout << "OK" << std::flush;
        return 0;
    }
    std::cout << "KO" << std::flush;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <bin path> <command> [args...]" << std::endl;
        return 1;
    }

    std::vector<MapCell> cells = MapReader::readMapFromBinary(argv[1]);
    if (cells.empty())
    {
        return 1;
    }
    Map map = Map(cells);
    // Créer l'instance Commands
    Commands commands(argc, argv, map);

    // Enregistrer les commandes
    commands.registerCommand("hasFourAdjacentCellsFree", hasFourAdjacentCellsFree);
    commands.registerCommand("getNeighbors", getNeighbors);
    commands.registerCommand("isLos", isLos);

    if (argc < 3)
    {
        commands.listCommands();
        return 1;
    }

    return commands.execute(argc, argv);
}