#pragma once

#include <iostream>
#include <vector>

#include "WorldGraph.hpp"
#include "Commands.hpp"

int findMap(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;

    if (args.size() < 1) {
        std::cerr << "Usage: findMap <mapId>" << std::endl;
        return 1;
    }

    try {
        uint32_t mapId = parseMapId(args[0]);
        auto mapData = worldGraph.findMap(mapId);

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

int getMapEdges(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;

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

int getWorldGraphStats(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;
    (void)args;

    std::cout << "{"
        << "\"mapCount\":" << worldGraph.getMapCount() << ","
        << "\"totalEdges\":" << worldGraph.getTotalEdgeCount() << ","
        << "\"totalTransitions\":" << worldGraph.getTotalTransitionCount() << ","
        << "\"averageEdgesPerMap\":"
        << (worldGraph.getMapCount() > 0 ?
            static_cast<double>(worldGraph.getTotalEdgeCount()) / worldGraph.getMapCount() : 0)
        << "}" << std::endl;
    return 0;
}

int listMapsInArea(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;

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

int getNeighborsDetailed(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;

    if (args.size() < 1) {
        std::cerr << "Usage: getNeighborsDetailed <mapId> [direction [amount]]" << std::endl;
        std::cerr << "  direction: 0 (EAST), 2 (SOUTH), 4 (WEST), 6 (NORTH)" << std::endl;
        std::cerr << "  amount: optional, number of neighbors to return (default: all)" << std::endl;
        return 1;
    }

    try {
        uint32_t mapId = parseMapId(args[0]);

        // Direction optionnelle
        int filterDirection = -1;
        if (args.size() >= 2) {
            filterDirection = std::stoi(args[1]);
            if (filterDirection != 0 && filterDirection != 2 &&
                filterDirection != 4 && filterDirection != 6) {
                std::cerr << "Error: Invalid direction. Must be 0, 2, 4, or 6" << std::endl;
                return 1;
            }
        }

        int amount = -1;
        // Amount optionnelle (non utilisée ici, on retourne tous les voisins)
        if (args.size() >= 3) {
            amount = std::stoi(args[2]);
            if (amount < 0) {
                std::cerr << "Error: Invalid amount. Must be a non-negative integer." << std::endl;
                return 1;
            }
            // Note: amount is not used in this implementation, we return all neighbors
        }

        const std::vector<WorldGraphEdge>* edges = worldGraph.getMapEdges(mapId);

        if (!edges) {
            std::cout << "[]" << std::flush;
            return 0;
        }
        // Structure pour stocker les voisins avec leurs transitions
        struct NeighborInfo {
            uint32_t mapId;
            uint8_t zoneId;
            std::vector<WorldGraphEdgeTransition> transitions;
        };

        std::vector<NeighborInfo> neighbors;

        // Parcourir tous les edges
        for (const auto& edge : *edges) {
            std::vector<WorldGraphEdgeTransition> filteredTransitions;

            // Filtrer les transitions par direction si nécessaire
            for (const auto& transition : edge.transitions) {
                if (filterDirection == -1 || transition.direction == filterDirection) {
                    filteredTransitions.push_back(transition);
                }
            }

            // Ajouter le voisin si il a des transitions valides
            if (!filteredTransitions.empty()) {
                neighbors.push_back({ edge.toMapId, edge.zoneId, filteredTransitions });
            }
        }

        // Retourner le résultat en JSON détaillé
        std::cout << "[";
        for (size_t i = 0; i < neighbors.size(); ++i) {
            if (i > 0) std::cout << ",";
            const auto& neighbor = neighbors[i];
            // Récupérer les données de la map cible
            const WorldGraphMapData* targetMapData = worldGraph.findMap(neighbor.mapId);
            const Vec2& targetPos = targetMapData ? targetMapData->position : Vec2{ -127, -127 };
            std::string targetPosStr = (targetMapData) ?
                "\"x\":" + std::to_string(static_cast<int>(targetPos.x)) +
                ",\"y\":" + std::to_string(static_cast<int>(targetPos.y)) + "," :
                "";

            std::cout << "{"
                << "\"toMapId\":" << neighbor.mapId << ","
                << targetPosStr
                << "\"zoneId\":" << static_cast<int>(neighbor.zoneId) << ","
                << "\"transitions\":[";

            for (size_t j = 0; j < neighbor.transitions.size(); ++j) {
                if (j > 0) std::cout << ",";
                const auto& trans = neighbor.transitions[j];
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
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int countMapWithPois(const std::string& /*programParam*/, const std::vector<std::string>& /*args*/, const WorldGraph& worldGraph) {
    auto wg = worldGraph.getWorldGraph();
    int count = 0;
    for (const auto& pair : wg) {
        const auto& edges = pair.second;
        if (std::find_if(edges.begin(), edges.end(), [](const WorldGraphEdge& edge) {
            return std::any_of(edge.transitions.begin(), edge.transitions.end(), [](const WorldGraphEdgeTransition& transition) {
                return (transition.direction == 0 || transition.direction == 2 || transition.direction == 4 || transition.direction == 6) && transition.skillId == -1;
                });
            }) != edges.end()) {
            ++count;
        }
    }
    std::cout << count << std::flush;
    return 0;
}

int getMapsToDirection(const std::string& programParam, const std::vector<std::string>& args, const WorldGraph& worldGraph) {
    (void)programParam;

    if (args.size() < 2) {
        std::cerr << "Usage: getMapsToDirections <mapId> <direction> [length]" << std::endl;
        return 1;
    }

    try {
        uint32_t mapId = parseMapId(args[0]);
        int direction = std::stoi(args[1]);
        if (direction < 0 || direction >= 8) {
            std::cerr << "Error: Invalid direction. Must be between 0 and 7." << std::endl;
            return 1;
        }
        int length = (args.size() > 2) ? std::stoi(args[2]) : 1;

        std::vector<uint32_t> result = {};
        while (length-- > 0) {
            const std::vector<WorldGraphEdge>* edges = worldGraph.getMapEdges(mapId);
            if (!edges) {
                break;
            }
            auto it = std::find_if(edges->begin(), edges->end(), [direction](const WorldGraphEdge& edge) {
                return std::any_of(edge.transitions.begin(), edge.transitions.end(), [direction](const WorldGraphEdgeTransition& transition) {
                    return transition.direction == direction;
                    });
                });
            if (it == edges->end()) {
                break; // Pas de transition dans cette direction
            }
            result.push_back(it->toMapId);
            mapId = it->toMapId; // Passer à la prochaine map
        }

        if (result.empty()) {
            std::cout << "[]" << std::flush;
            return 0;
        }

        std::cout << "[";
        for (size_t i = 0; i < result.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << result[i];
        }
        std::cout << "]" << std::flush;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}