#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "Commands.hpp"
#include "DfbException.hpp"
#include "Map.hpp"
#include "MapCell.hpp"
#include "MapReader.hpp"
#include "WorldGraph.hpp"
#include <set>

// Déclaration des fonctions de commandes existantes
std::set<int> getOccupiedCellsFromArgs(const std::vector<std::string>& args, int startIndex);
int hasFourAdjacentCellsFree(const std::string& programParam, const std::vector<std::string>& args, const Map& map);
int getNeighbors(const std::string& programParam, const std::vector<std::string>& args, const Map& map);
int isLos(const std::string& programParam, const std::vector<std::string>& args, const Map& map);
int getLosCells(const std::string& programParam, const std::vector<std::string>& args, const Map& map);
int getMapDataJson(const std::string& programParam, const std::vector<std::string>& args, const Map& map);

// Déclaration pour enregistrer les commandes WorldGraph
void registerWorldGraphCommands(Commands& commands, const WorldGraph& worldGraph);

// Implémentation des commandes Map existantes...
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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <type> <bin path> <command> [args...]" << std::endl;
    std::cout << "  type: 'map' or 'worldgraph'" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << " map /path/to/map.bin getNeighbors 123" << std::endl;
    std::cout << "  " << programName << " worldgraph /path/to/worldgraph.bin findMap 123456" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "DFB Map Tools - Version 1.0" << std::endl;
    if (argc < 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    std::string type = argv[1];
    std::string binPath = argv[2];

    // Map vide par défaut (pour les commandes worldgraph qui ne l'utilisent pas)
    Map emptyMap = Map(std::vector<MapCell>{});

    if (type == "map") {
        // Mode Map - comportement original
        std::vector<MapCell> cells = MapReader::readMapFromBinary(binPath);
        if (cells.empty())
        {
            return 1;
        }
        Map map = Map(cells);

        // Créer l'instance Commands
        Commands commands(argc - 2, argv + 2, map);

        // Enregistrer les commandes Map
        commands.registerCommand("hasFourAdjacentCellsFree", hasFourAdjacentCellsFree);
        commands.registerCommand("getNeighbors", getNeighbors);
        commands.registerCommand("isLos", isLos);
        commands.registerCommand("getLosCells", getLosCells);
        commands.registerCommand("getMapDataJson", getMapDataJson);

        if (argc < 4)
        {
            std::cout << "Available map commands:" << std::endl;
            commands.listCommands();
            return 1;
        }

        // Ajuster les arguments pour Commands (enlever type et binPath)
        return commands.execute(argc - 2, argv + 2);

    } else if (type == "worldgraph") {
        // Mode WorldGraph
        std::unique_ptr<WorldGraph> worldGraph = std::make_unique<WorldGraph>();

        if (!worldGraph->loadFromBinary(binPath)) {
            std::cerr << "Failed to load worldgraph from " << binPath << std::endl;
            return 1;
        }

        // Créer l'instance Commands avec la map vide
        Commands commands(argc - 2, argv + 2, emptyMap);

        // Enregistrer les commandes WorldGraph
        registerWorldGraphCommands(commands, *worldGraph);

        if (argc < 4)
        {
            std::cout << "Available worldgraph commands:" << std::endl;
            commands.listCommands();
            return 1;
        }

        // Ajuster les arguments pour Commands (enlever type et binPath)
        return commands.execute(argc - 2, argv + 2);

    } else {
        std::cerr << "Error: Unknown type '" << type << "'. Use 'map' or 'worldgraph'." << std::endl;
        printUsage(argv[0]);
        return 1;
    }
}