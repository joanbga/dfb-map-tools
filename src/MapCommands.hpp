#include <iostream>
#include <set>
#include <vector>
#include <string>
#include "Map.hpp"
#include "DfbException.hpp"

std::set<int> getOccupiedCellsFromArgs(const std::vector<std::string>& args, int startIndex)
{
    std::set<int> occupiedCells;
    for (size_t i = startIndex; i < args.size(); ++i)
    {
        try
        {
            int occupiedCellId = std::stoi(args[i]);
            occupiedCells.insert(occupiedCellId);
        }
        catch (const std::invalid_argument& e)
        {
            std::cerr << "Error: '" << args[i] << "' is not a valid cellId" << std::flush;
            return {};
        }
        catch (const std::out_of_range& e)
        {
            std::cerr << "Error: '" << args[i] << "' too big for cellId" << std::flush;
            return {};
        }
    }
    return occupiedCells;
}

int hasFourAdjacentCellsFree(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map)
{
    (void)programParam;
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

int getNeighbors(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map)
{
    (void)programParam;
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
    catch (const DfbException& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

int isLos(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map)
{
    (void)programParam;
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

int getLosCells(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map) {
    (void)programParam;
    if (args.size() < 1)
    {
        std::cerr << "Usage: getLosCells <startCellId> [occupiedCells...]" << std::endl;
        return 1;
    }

    int startCellId;
    try
    {
        startCellId = std::stoi(args[0]);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Error: '" << args[0] << "' is not a valid cellId" << std::endl;
        return 1;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Error: '" << args[0] << "' is too big for cellId" << std::endl;
        return 1;
    }

    if (!map.cellExists(startCellId))
    {
        std::cerr << "Error: Cell " << startCellId << " does not exist" << std::endl;
        return 1;
    }

    std::set<int> occupiedCells = getOccupiedCellsFromArgs(args, 1);
    std::vector<int> losCells;

    for (size_t i = 0; i < map.getCellCount(); ++i)
    {
        if (static_cast<int>(i) == startCellId)
        {
            continue;
        }

        const MapCell* cell = map.getCellByNumber(i);
        if (cell && cell->isWalkable())
        {
            if (map.isLos(startCellId, i, occupiedCells))
            {
                losCells.push_back(i);
            }
        }
    }

    std::cout << "[";
    for (size_t i = 0; i < losCells.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << losCells[i];
    }
    std::cout << "]" << std::flush;

    return 0;
}

int getMapDataJson(const std::string& programParam, const std::vector<std::string>& args, const Map& map) {
    (void)programParam;
    (void)args;
    std::cout << "{\"cellsData\":[";
    for (size_t i = 0; i < map.getCellCount(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        const MapCell* cell = map.getCellByNumber(i);
        if (cell)
        {
            std::cout << "{"
                << "\"cellNumber\":" << cell->cellNumber << ","
                << "\"speed\":" << cell->speed << ","
                << "\"mapChangeData\":" << cell->mapChangeData << ","
                << "\"moveZone\":" << cell->moveZone << ","
                << "\"linkedZone\":" << cell->linkedZone << ","
                << "\"mov\":" << cell->mov << ","
                << "\"los\":" << cell->los << ","
                << "\"nonWalkableDuringFight\":" << cell->nonWalkableDuringFight << ","
                << "\"nonWalkableDuringRP\":" << cell->nonWalkableDuringRP << ","
                << "\"farmCell\":" << cell->farmCell << ","
                << "\"visible\":" << cell->visible << ","
                << "\"havenbagCell\":" << cell->havenbagCell << ","
                << "\"floor\":" << cell->floor << ","
                << "\"red\":" << cell->red << ","
                << "\"blue\":" << cell->blue <<
                "}";
        }
    }
    std::cout << "]}" << std::flush;
    return 0;
}