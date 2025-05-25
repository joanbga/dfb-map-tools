#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>

#include "WorldGraph.hpp"
#include "Commands.hpp"

// Structure pour représenter un nœud dans le pathfinding
struct PathNode {
    uint32_t mapId;
    uint32_t parentMapId;
    int direction;  // -1 pour téléportation, 0-7 pour marche
    int cellId;     // cellId pour marche, -1 pour téléportation
    bool isTeleport;
    double gCost;  // Coût depuis le départ
    double hCost;  // Heuristique vers l'arrivée
    double fCost() const { return gCost + hCost; }
};

// Comparateur pour la priority queue (min heap basé sur fCost)
struct PathNodeCompare {
    bool operator()(const PathNode* a, const PathNode* b) const {
        return a->fCost() > b->fCost();
    }
};

// Structure pour le résultat du path
struct PathStep {
    uint32_t toMapId;
    int cellId;
    std::string type;  // "teleport", "walk", ou "start"
    int direction;     // -1 pour teleport, 0-7 pour walk
};

// Fonction helper pour parser un mapId
uint32_t parseMapId(const std::string& arg);

// Calcul de la distance heuristique entre deux maps (distance Manhattan)
inline double calculateHeuristic(Vec2 from, Vec2 to) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

// Fonction de pathfinding basique sans téléportation
std::vector<PathStep> findBasicPath(const WorldGraph& worldGraph,
    uint32_t startMapId,
    uint32_t endMapId) {

    if (!worldGraph.mapExists(startMapId) || !worldGraph.mapExists(endMapId)) {
        return {};
    }

    if (startMapId == endMapId) {
        return { {startMapId, -1, "start", -1} };
    }

    // Structures pour A*
    std::unordered_map<uint32_t, PathNode*> allNodes;
    std::unordered_set<uint32_t> closedSet;
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeCompare> openSet;

    // Créer le nœud de départ
    PathNode* startNode = new PathNode{
        startMapId,
        0,
        -1,
        -1,
        false,
        0.0,
        calculateHeuristic(worldGraph.getMapCoordinates(startMapId),
                          worldGraph.getMapCoordinates(endMapId))
    };
    allNodes[startMapId] = startNode;
    openSet.push(startNode);

    PathNode* endNode = nullptr;

    // A* algorithm
    while (!openSet.empty()) {
        PathNode* currentNode = openSet.top();
        openSet.pop();

        if (currentNode->mapId == endMapId) {
            endNode = currentNode;
            break;
        }

        closedSet.insert(currentNode->mapId);

        // Explorer les voisins par marche
        const std::vector<WorldGraphEdge>* edges = worldGraph.getMapEdges(currentNode->mapId);
        if (edges) {
            for (const auto& edge : *edges) {
                if (closedSet.find(edge.toMapId) != closedSet.end()) {
                    continue;
                }

                if (edge.transitions.empty()) continue;

                // Utiliser la première transition
                const auto& transition = edge.transitions[0];
                double newGCost = currentNode->gCost + 1.0;

                auto it = allNodes.find(edge.toMapId);
                if (it != allNodes.end() && it->second->gCost <= newGCost) {
                    continue;
                }

                PathNode* neighborNode;
                if (it == allNodes.end()) {
                    neighborNode = new PathNode{
                        edge.toMapId,
                        currentNode->mapId,
                        transition.direction,
                        transition.cellId,
                        false,
                        newGCost,
                        calculateHeuristic(worldGraph.getMapCoordinates(edge.toMapId),
                                         worldGraph.getMapCoordinates(endMapId))
                    };
                    allNodes[edge.toMapId] = neighborNode;
                } else {
                    neighborNode = it->second;
                    neighborNode->parentMapId = currentNode->mapId;
                    neighborNode->direction = transition.direction;
                    neighborNode->cellId = transition.cellId;
                    neighborNode->isTeleport = false;
                    neighborNode->gCost = newGCost;
                }

                openSet.push(neighborNode);
            }
        }
    }

    // Reconstruire le chemin si trouvé
    std::vector<PathStep> path;
    if (endNode) {
        uint32_t current = endMapId;

        while (current != 0) {
            PathNode* node = allNodes[current];
            std::string type = node->isTeleport ? "teleport" : "walk";
            path.push_back({ current, node->cellId, type, node->direction });

            if (current == startMapId) break;
            current = node->parentMapId;
        }

        std::reverse(path.begin(), path.end());

        if (!path.empty()) {
            path[0].type = "start";
            path[0].cellId = -1;
        }
    }

    // Nettoyer la mémoire
    for (auto& pair : allNodes) {
        delete pair.second;
    }

    return path;
}

// Fonction principale de pathfinding avec téléportations
std::vector<PathStep> findPath(const WorldGraph& worldGraph,
    uint32_t startMapId,
    uint32_t endMapId,
    const std::unordered_set<uint32_t>& teleportMaps) {

    // Cas simple : pas de téléportation
    if (teleportMaps.empty()) {
        return findBasicPath(worldGraph, startMapId, endMapId);
    }

    // Trouver le chemin direct sans téléportation
    std::vector<PathStep> directPath = findBasicPath(worldGraph, startMapId, endMapId);
    size_t bestPathLength = directPath.size();
    std::vector<PathStep> bestPath = directPath;

    // Si pas de chemin direct, bestPathLength sera 0, on met une valeur très grande
    if (bestPathLength == 0) {
        bestPathLength = std::numeric_limits<size_t>::max();
    }

    // Pour chaque map de téléportation, calculer le chemin via cette téléportation
    for (uint32_t teleportMapId : teleportMaps) {
        if (!worldGraph.mapExists(teleportMapId)) {
            continue;
        }

        // Chemin: start -> teleport -> end
        std::vector<PathStep> pathViaTeleport;

        // Étape 1: Départ
        pathViaTeleport.push_back({ startMapId, -1, "start", -1 });

        // Étape 2: Téléportation (sauf si on est déjà sur la map de téléportation)
        if (startMapId != teleportMapId) {
            pathViaTeleport.push_back({ teleportMapId, -1, "teleport", -1 });
        }

        // Étape 3: Marche depuis la téléportation jusqu'à destination (si nécessaire)
        if (teleportMapId != endMapId) {
            std::vector<PathStep> pathFromTeleport = findBasicPath(worldGraph, teleportMapId, endMapId);

            // Si un chemin existe depuis la téléportation
            if (!pathFromTeleport.empty()) {
                // Ignorer le premier élément (qui est la map de téléportation elle-même)
                for (size_t i = 1; i < pathFromTeleport.size(); ++i) {
                    pathViaTeleport.push_back(pathFromTeleport[i]);
                }

                // Comparer avec le meilleur chemin trouvé
                if (pathViaTeleport.size() < bestPathLength) {
                    bestPathLength = pathViaTeleport.size();
                    bestPath = pathViaTeleport;
                }
            }
        } else {
            // La téléportation nous amène directement à destination
            if (pathViaTeleport.size() < bestPathLength) {
                bestPathLength = pathViaTeleport.size();
                bestPath = pathViaTeleport;
            }
        }
    }

    return bestPath;
}

// Commande de pathfinding
inline int pathfinding(const std::string& programParam, const std::vector<std::string>& args,
    const WorldGraph& worldGraph) {
    (void)programParam;

    if (args.size() < 2) {
        std::cerr << "Usage: pathfinding <startMapId> <endMapId> [teleportMapId1] [teleportMapId2] ..." << std::endl;
        return 1;
    }

    try {
        uint32_t startMapId = parseMapId(args[0]);
        uint32_t endMapId = parseMapId(args[1]);

        // Parser les maps de téléportation
        std::unordered_set<uint32_t> teleportMaps;
        for (size_t i = 2; i < args.size(); ++i) {
            teleportMaps.insert(parseMapId(args[i]));
        }

        // Trouver le chemin
        std::vector<PathStep> path = findPath(worldGraph, startMapId, endMapId, teleportMaps);

        // Afficher le résultat en JSON
        std::cout << "[";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << "\n  {"
                << "\"toMapId\":" << path[i].toMapId << ","
                << "\"cellId\":" << path[i].cellId << ","
                << "\"type\":\"" << path[i].type << "\","
                << "\"direction\":" << path[i].direction
                << "}";
        }
        if (!path.empty()) std::cout << "\n";
        std::cout << "]" << std::flush;

        return path.empty() ? 1 : 0;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}