#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <memory>
#include <fstream>
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
    bool isLoaded = false; // Indique si les edges ont été chargés
};

// Structure pour stocker les infos de l'index
struct MapIndexEntry {
    uint32_t mapId;
    uint32_t dataOffset;
    Vec2 position;      // Position stockée directement dans l'index pour éviter de charger les données
    uint16_t edgeCount; // Nombre d'edges stocké pour les stats
};

class WorldGraph {
private:
    mutable std::string m_filepath;
    mutable std::unordered_map<uint32_t, MapIndexEntry> m_index; // Index complet
    mutable std::unordered_map<uint32_t, WorldGraphMapData> m_mapData; // Données chargées
    mutable std::unordered_map<uint32_t, Vec2> m_mapCoordinates; // Toutes les coordonnées (depuis l'index)
    mutable std::unordered_map<uint32_t, std::vector<WorldGraphEdge>> m_worldGraph;

    uint32_t m_mapCount = 0;
    mutable bool m_indexLoaded = false;

    // Charge uniquement les données d'une map spécifique
    bool loadMapData(uint32_t mapId) const;

    // WorldGraphReader a besoin d'accéder aux membres privés
    friend class WorldGraphReader;

public:
    WorldGraph() = default;

    // Charge uniquement l'index du fichier
    bool loadIndex(const std::string& filepath);

    // Charge tout le fichier (comportement actuel)
    bool loadFromBinary(const std::string& filepath);

    // Recherche une map par ID (charge les données si nécessaire)
    const WorldGraphMapData* findMap(uint32_t mapId) const;

    // Récupère les coordonnées d'une map (depuis l'index, pas besoin de charger)
    Vec2 getMapCoordinates(uint32_t mapId) const;

    // Récupère les edges sortants d'une map (charge si nécessaire)
    const std::vector<WorldGraphEdge>* getMapEdges(uint32_t mapId) const;

    // Vérifie si une map existe (depuis l'index)
    bool mapExists(uint32_t mapId) const;

    // Récupère toutes les maps (depuis l'index)
    std::vector<uint32_t> getAllMapIds() const;

    // Stats (depuis l'index, pas besoin de charger toutes les données)
    size_t getMapCount() const { return m_mapCount; }
    size_t getTotalEdgeCount() const;
    size_t getTotalTransitionCount() const;

    std::unordered_map<uint32_t, std::vector<WorldGraphEdge>> getWorldGraph() const { return m_worldGraph; }

    // Précharge un ensemble de maps
    void preloadMaps(const std::vector<uint32_t>& mapIds);
};