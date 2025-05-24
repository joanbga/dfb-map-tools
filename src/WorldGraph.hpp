#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>
#include "Direction.hpp"

struct Vec2 {
    int8_t x;
    int8_t y;
};

struct WorldGraphEdgeTransition {
    uint8_t type;
    uint8_t direction;
    int16_t skillId;
    uint32_t transitionMapId;
    uint16_t cellId;
};

struct WorldGraphEdge {
    uint32_t toMapId;
    uint8_t zoneId;
    std::vector<WorldGraphEdgeTransition> transitions;
};

struct WorldGraphMapData {
    uint32_t mapId;
    Vec2 position;
    std::vector<WorldGraphEdge> edges;
};

class WorldGraph {

public:
    std::unordered_map<uint32_t, WorldGraphMapData> m_mapData;
    std::unordered_map<uint32_t, Vec2> m_mapCoordinates;
    std::unordered_map<uint32_t, std::vector<WorldGraphEdge>> m_worldGraph;
    WorldGraph() = default;

    // Charge le worldgraph depuis le fichier binaire
    bool loadFromBinary(const std::string& filepath);

    // Recherche une map par ID
    const WorldGraphMapData* findMap(uint32_t mapId) const;

    // Récupère les coordonnées d'une map
    Vec2 getMapCoordinates(uint32_t mapId) const;

    // Récupère les edges sortants d'une map
    const std::vector<WorldGraphEdge>* getMapEdges(uint32_t mapId) const;

    // Vérifie si une map existe
    bool mapExists(uint32_t mapId) const;

    // Récupère toutes les maps
    std::vector<uint32_t> getAllMapIds() const;

    // Stats
    size_t getMapCount() const { return m_mapData.size(); }
    size_t getTotalEdgeCount() const;
    size_t getTotalTransitionCount() const;
};