#include "WorldGraph.hpp"
#include "WorldGraphReader.hpp"
#include <algorithm>
#include <iostream>

bool WorldGraph::loadIndex(const std::string& filepath) {
    m_filepath = filepath;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Lire le header
    WorldGraphReader::FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (!file.good()) {
        std::cerr << "Failed to read header from file" << std::endl;
        return false;
    }

    // Valider le header
    if (header.magic != WorldGraphReader::MAGIC) {
        std::cerr << "Invalid magic number: " << std::hex << header.magic
            << " (expected " << WorldGraphReader::MAGIC << ")" << std::dec << std::endl;
        return false;
    }

    if (header.version != WorldGraphReader::SUPPORTED_VERSION) {
        std::cerr << "Unsupported version: " << header.version
            << " (supported: " << WorldGraphReader::SUPPORTED_VERSION << ")" << std::endl;
        return false;
    }

    m_mapCount = header.mapCount;

    // Se positionner à l'index
    file.seekg(header.indexOffset);

    // Lire l'index
    std::vector<WorldGraphReader::IndexEntry> rawIndex(header.mapCount);
    file.read(reinterpret_cast<char*>(rawIndex.data()),
        header.mapCount * sizeof(WorldGraphReader::IndexEntry));

    // Pour chaque entrée de l'index, lire juste la position et le nombre d'edges
    for (const auto& entry : rawIndex) {
        MapIndexEntry indexEntry;
        indexEntry.mapId = entry.mapId;
        indexEntry.dataOffset = entry.dataOffset;

        // Lire juste la position et le nombre d'edges
        file.seekg(entry.dataOffset);
        file.read(reinterpret_cast<char*>(&indexEntry.position.x), sizeof(int8_t));
        file.read(reinterpret_cast<char*>(&indexEntry.position.y), sizeof(int8_t));
        file.read(reinterpret_cast<char*>(&indexEntry.edgeCount), sizeof(uint16_t));

        m_index[entry.mapId] = indexEntry;
        m_mapCoordinates[entry.mapId] = indexEntry.position;
    }

    m_indexLoaded = true;
    return true;
}

bool WorldGraph::loadMapData(uint32_t mapId) const {
    if (!m_indexLoaded) {
        return false;
    }

    auto it = m_index.find(mapId);
    if (it == m_index.end()) {
        return false;
    }

    // Si déjà chargé, ne pas recharger
    auto dataIt = m_mapData.find(mapId);
    if (dataIt != m_mapData.end() && dataIt->second.isLoaded) {
        return true;
    }

    std::ifstream file(m_filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const MapIndexEntry& indexEntry = it->second;
    file.seekg(indexEntry.dataOffset);

    WorldGraphMapData mapData;
    mapData.mapId = mapId;

    // Lire position (on l'a déjà mais on doit avancer le pointeur)
    file.read(reinterpret_cast<char*>(&mapData.position.x), sizeof(int8_t));
    file.read(reinterpret_cast<char*>(&mapData.position.y), sizeof(int8_t));

    // Lire nombre d'edges
    uint16_t edgeCount;
    file.read(reinterpret_cast<char*>(&edgeCount), sizeof(uint16_t));

    // Lire les edges
    mapData.edges.reserve(edgeCount);
    for (uint16_t i = 0; i < edgeCount; ++i) {
        WorldGraphEdge edge;

        file.read(reinterpret_cast<char*>(&edge.toMapId), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&edge.zoneId), sizeof(uint8_t));

        uint8_t transitionCount;
        file.read(reinterpret_cast<char*>(&transitionCount), sizeof(uint8_t));

        edge.transitions.reserve(transitionCount);
        for (uint8_t j = 0; j < transitionCount; ++j) {
            WorldGraphEdgeTransition transition;

            file.read(reinterpret_cast<char*>(&transition.type), sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(&transition.direction), sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(&transition.skillId), sizeof(int16_t));
            file.read(reinterpret_cast<char*>(&transition.transitionMapId), sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&transition.cellId), sizeof(uint16_t));

            edge.transitions.push_back(transition);
        }

        mapData.edges.push_back(edge);
    }

    mapData.isLoaded = true;

    // Stocker les données
    m_mapData[mapId] = mapData;
    if (!mapData.edges.empty()) {
        m_worldGraph[mapId] = mapData.edges;
    }

    return true;
}

bool WorldGraph::loadFromBinary(const std::string& filepath) {
    m_filepath = filepath;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Lire le header
    WorldGraphReader::FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (!file.good()) {
        std::cerr << "Failed to read header from file" << std::endl;
        return false;
    }

    // Valider le header
    if (header.magic != WorldGraphReader::MAGIC) {
        std::cerr << "Invalid magic number: " << std::hex << header.magic
            << " (expected " << WorldGraphReader::MAGIC << ")" << std::dec << std::endl;
        return false;
    }

    if (header.version != WorldGraphReader::SUPPORTED_VERSION) {
        std::cerr << "Unsupported version: " << header.version
            << " (supported: " << WorldGraphReader::SUPPORTED_VERSION << ")" << std::endl;
        return false;
    }

    m_mapCount = header.mapCount;

    // Se positionner à l'index
    file.seekg(header.indexOffset);

    // Lire l'index
    std::vector<WorldGraphReader::IndexEntry> index(header.mapCount);
    file.read(reinterpret_cast<char*>(index.data()),
        header.mapCount * sizeof(WorldGraphReader::IndexEntry));

    // Lire les données de chaque map
    for (const auto& entry : index) {
        file.seekg(entry.dataOffset);

        WorldGraphMapData mapData;
        mapData.mapId = entry.mapId;

        // Lire position
        file.read(reinterpret_cast<char*>(&mapData.position.x), sizeof(int8_t));
        file.read(reinterpret_cast<char*>(&mapData.position.y), sizeof(int8_t));

        // Lire nombre d'edges
        uint16_t edgeCount;
        file.read(reinterpret_cast<char*>(&edgeCount), sizeof(uint16_t));

        // Lire les edges
        mapData.edges.reserve(edgeCount);
        for (uint16_t i = 0; i < edgeCount; ++i) {
            WorldGraphEdge edge;

            // Lire toMapId et zoneId
            file.read(reinterpret_cast<char*>(&edge.toMapId), sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&edge.zoneId), sizeof(uint8_t));

            // Lire nombre de transitions
            uint8_t transitionCount;
            file.read(reinterpret_cast<char*>(&transitionCount), sizeof(uint8_t));

            // Lire les transitions
            edge.transitions.reserve(transitionCount);
            for (uint8_t j = 0; j < transitionCount; ++j) {
                WorldGraphEdgeTransition transition;

                file.read(reinterpret_cast<char*>(&transition.type), sizeof(uint8_t));
                file.read(reinterpret_cast<char*>(&transition.direction), sizeof(uint8_t));
                file.read(reinterpret_cast<char*>(&transition.skillId), sizeof(int16_t));
                file.read(reinterpret_cast<char*>(&transition.transitionMapId), sizeof(uint32_t));
                file.read(reinterpret_cast<char*>(&transition.cellId), sizeof(uint16_t));

                edge.transitions.push_back(transition);
            }

            mapData.edges.push_back(edge);
        }

        mapData.isLoaded = true;

        // Stocker dans les structures internes de WorldGraph
        m_mapData[mapData.mapId] = mapData;
        m_mapCoordinates[mapData.mapId] = mapData.position;

        // Créer la liste d'adjacence pour le graphe
        if (!mapData.edges.empty()) {
            m_worldGraph[mapData.mapId] = mapData.edges;
        }

        // Mettre à jour l'index
        MapIndexEntry indexEntry;
        indexEntry.mapId = entry.mapId;
        indexEntry.dataOffset = entry.dataOffset;
        indexEntry.position = mapData.position;
        indexEntry.edgeCount = edgeCount;
        m_index[entry.mapId] = indexEntry;
    }

    if (!file.good() && !file.eof()) {
        std::cerr << "Error reading file" << std::endl;
        return false;
    }

    m_indexLoaded = true;
    return true;
}

const WorldGraphMapData* WorldGraph::findMap(uint32_t mapId) const {
    // Vérifier si la map existe dans l'index
    if (m_index.find(mapId) == m_index.end()) {
        return nullptr;
    }

    // Charger les données si nécessaire
    auto it = m_mapData.find(mapId);
    if (it == m_mapData.end() || !it->second.isLoaded) {
        if (!loadMapData(mapId)) {
            return nullptr;
        }
        it = m_mapData.find(mapId);
    }

    return &it->second;
}

Vec2 WorldGraph::getMapCoordinates(uint32_t mapId) const {
    auto it = m_mapCoordinates.find(mapId);
    if (it != m_mapCoordinates.end()) {
        return it->second;
    }
    return { 0, 0 };
}

const std::vector<WorldGraphEdge>* WorldGraph::getMapEdges(uint32_t mapId) const {
    // Charger les données si nécessaire
    const WorldGraphMapData* mapData = findMap(mapId);
    if (!mapData) {
        return nullptr;
    }

    auto it = m_worldGraph.find(mapId);
    if (it != m_worldGraph.end()) {
        return &it->second;
    }
    return nullptr;
}

bool WorldGraph::mapExists(uint32_t mapId) const {
    return m_index.find(mapId) != m_index.end();
}

std::vector<uint32_t> WorldGraph::getAllMapIds() const {
    std::vector<uint32_t> mapIds;
    mapIds.reserve(m_index.size());

    for (const auto& pair : m_index) {
        mapIds.push_back(pair.first);
    }

    std::sort(mapIds.begin(), mapIds.end());
    return mapIds;
}

size_t WorldGraph::getTotalEdgeCount() const {
    size_t count = 0;
    for (const auto& pair : m_index) {
        count += pair.second.edgeCount;
    }
    return count;
}

size_t WorldGraph::getTotalTransitionCount() const {
    // Pour cette stat, on doit charger toutes les données
    size_t count = 0;
    for (const auto& pair : m_worldGraph) {
        for (const auto& edge : pair.second) {
            count += edge.transitions.size();
        }
    }

    // Si toutes les données ne sont pas chargées, on ne peut pas donner le compte exact
    if (m_mapData.size() < m_index.size()) {
        std::cerr << "Warning: Not all map data loaded, transition count may be incomplete" << std::endl;
    }

    return count;
}

void WorldGraph::preloadMaps(const std::vector<uint32_t>& mapIds) {
    for (uint32_t mapId : mapIds) {
        loadMapData(mapId);
    }
}