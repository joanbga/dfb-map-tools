#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "Commands.hpp"
#include "WorldGraph.hpp"

// Fonctions de commandes WorldGraph
int findMap(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph);
int getMapEdges(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph);
int getWorldGraphStats(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph);
int listMapsInArea(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph);

// Fonction helper pour parser un mapId depuis les arguments
uint32_t parseMapId(const std::string& arg) {
    try {
        // Support pour les nombres hexadécimaux (préfixés par 0x)
        if (arg.size() > 2 && arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
            return std::stoul(arg, nullptr, 16);
        }
        return std::stoul(arg);
    }
    catch (const std::exception& e) {
        throw std::invalid_argument("Invalid mapId: " + arg);
    }
}

int findMap(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph) {
    (void)programParam;
    (void)map;

    if (args.size() < 1) {
        std::cerr << "Usage: findMap <mapId>" << std::endl;
        return 1;
    }

    try {
        uint32_t mapId = parseMapId(args[0]);
        const WorldGraphMapData* mapData = worldGraph.findMap(mapId);

        if (mapData) {
            std::cout << "{"
                << "\"mapId\":" << mapData->mapId << ","
                << "\"position\":{\"x\":" << static_cast<int>(mapData->position.x)
                << ",\"y\":" << static_cast<int>(mapData->position.y) << "},"
                << "\"edgeCount\":" << mapData->edges.size()
                << "}" << std::flush;
            return 0;
        } else {
            std::cout << "null" << std::flush;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int getMapEdges(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph) {
    (void)programParam;
    (void)map;

    if (args.size() < 1) {
        std::cerr << "Usage: getMapEdges <mapId>" << std::endl;
        return 1;
    }

    try {
        uint32_t mapId = parseMapId(args[0]);
        const std::vector<WorldGraphEdge>* edges = worldGraph.getMapEdges(mapId);

        if (edges) {
            std::cout << "[";
            for (size_t i = 0; i < edges->size(); ++i) {
                if (i > 0) std::cout << ",";
                const auto& edge = (*edges)[i];
                std::cout << "{"
                    << "\"toMapId\":" << edge.toMapId << ","
                    << "\"zoneId\":" << static_cast<int>(edge.zoneId) << ","
                    << "\"transitions\":[";

                for (size_t j = 0; j < edge.transitions.size(); ++j) {
                    if (j > 0) std::cout << ",";
                    const auto& trans = edge.transitions[j];
                    std::cout << "{"
                        << "\"type\":" << static_cast<int>(trans.type) << ","
                        << "\"direction\":" << static_cast<int>(trans.direction) << ","
                        << "\"skillId\":" << trans.skillId << ","
                        << "\"transitionMapId\":" << trans.transitionMapId << ","
                        << "\"cellId\":" << trans.cellId
                        << "}";
                }

                std::cout << "]}";
            }
            std::cout << "]" << std::flush;
            return 0;
        } else {
            std::cout << "[]" << std::flush;
            return 0;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int getWorldGraphStats(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph) {
    (void)programParam;
    (void)args;
    (void)map;

    std::cout << "{"
        << "\"mapCount\":" << worldGraph.getMapCount() << ","
        << "\"totalEdges\":" << worldGraph.getTotalEdgeCount() << ","
        << "\"totalTransitions\":" << worldGraph.getTotalTransitionCount() << ","
        << "\"averageEdgesPerMap\":"
        << (worldGraph.getMapCount() > 0 ?
            static_cast<double>(worldGraph.getTotalEdgeCount()) / worldGraph.getMapCount() : 0)
        << "}" << std::flush;
    return 0;
}

int listMapsInArea(const std::string& programParam, const std::vector<std::string>& args,
    const Map& map, const WorldGraph& worldGraph) {
    (void)programParam;
    (void)map;

    if (args.size() < 4) {
        std::cerr << "Usage: listMapsInArea <minX> <minY> <maxX> <maxY>" << std::endl;
        return 1;
    }

    try {
        int minX = std::stoi(args[0]);
        int minY = std::stoi(args[1]);
        int maxX = std::stoi(args[2]);
        int maxY = std::stoi(args[3]);

        std::vector<uint32_t> mapsInArea;

        for (uint32_t mapId : worldGraph.getAllMapIds()) {
            Vec2 pos = worldGraph.getMapCoordinates(mapId);
            if (pos.x >= minX && pos.x <= maxX &&
                pos.y >= minY && pos.y <= maxY) {
                mapsInArea.push_back(mapId);
            }
        }

        std::cout << "[";
        for (size_t i = 0; i < mapsInArea.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << mapsInArea[i];
        }
        std::cout << "]" << std::flush;

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Enregistrer toutes les commandes WorldGraph
void registerWorldGraphCommands(Commands& commands, const WorldGraph& worldGraph) {
    commands.registerCommand("findMap",
        [&worldGraph](const std::string& p, const std::vector<std::string>& a, const Map& m) {
            return findMap(p, a, m, worldGraph);
        });

    commands.registerCommand("getMapEdges",
        [&worldGraph](const std::string& p, const std::vector<std::string>& a, const Map& m) {
            return getMapEdges(p, a, m, worldGraph);
        });

    commands.registerCommand("getWorldGraphStats",
        [&worldGraph](const std::string& p, const std::vector<std::string>& a, const Map& m) {
            return getWorldGraphStats(p, a, m, worldGraph);
        });

    commands.registerCommand("listMapsInArea",
        [&worldGraph](const std::string& p, const std::vector<std::string>& a, const Map& m) {
            return listMapsInArea(p, a, m, worldGraph);
        });
}