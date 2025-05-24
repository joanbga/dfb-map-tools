#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include "Commands.hpp"
#include "DfbException.hpp"
#include "Map.hpp"
#include "MapCell.hpp"
#include "MapReader.hpp"
#include "WorldGraph.hpp"
#include "WorldGraphCommands.hpp"
#include "MapCommands.hpp"



void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <type> <bin path> <command> [args...]" << std::endl;
    std::cout << "  type: 'map' or 'worldgraph'" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << " map /path/to/map.bin getNeighbors 123" << std::endl;
    std::cout << "  " << programName << " worldgraph /path/to/worldgraph.bin findMap 123456" << std::endl;
}

int main(int argc, char* argv[])
{
    // Forcer le flush automatique pour debug
    std::cout.setf(std::ios::unitbuf);
    std::cerr.setf(std::ios::unitbuf);

    if (argc < 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    std::string type = argv[1];
    std::string binPath = argv[2];

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
        // std::cout << "Loading worldgraph from: " << binPath << std::endl;
        WorldGraph worldGraph = WorldGraph();

        if (!worldGraph.loadFromBinary(binPath)) {
            std::cerr << "Failed to load worldgraph from " << binPath << std::flush;
            return 1;
        }

        // Créer l'instance Commands avec la map vide
        Commands commands(argc - 2, argv + 2, worldGraph);

        commands.registerCommand("findMap", findMap);
        commands.registerCommand("getMapEdges", getMapEdges);
        commands.registerCommand("getWorldGraphStats", getWorldGraphStats);
        commands.registerCommand("listMapsInArea", listMapsInArea);

        if (argc < 4)
        {
            std::cerr << "Available worldgraph commands:" << std::endl;
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