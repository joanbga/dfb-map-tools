#include "WorldGraphReader.hpp"
#include "WorldGraph.hpp"
#include <fstream>
#include <iostream>
// #include <istream>
#include <vector>

bool WorldGraphReader::validateHeader(const FileHeader& header) {
    if (header.magic != MAGIC) {
        std::cerr << "Invalid magic number: " << std::hex << header.magic
            << " (expected " << MAGIC << ")" << std::dec << std::endl;
        return false;
    }

    if (header.version != SUPPORTED_VERSION) {
        std::cerr << "Unsupported version: " << header.version
            << " (supported: " << SUPPORTED_VERSION << ")" << std::endl;
        return false;
    }

    return true;
}

bool WorldGraphReader::readWorldGraphFromBinary(const std::string& filepath, WorldGraph& worldGraph) {
    // std::cerr << "Attempting to open file: " << filepath << std::endl;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Lire le header
    FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));

    if (!file.good()) {
        std::cerr << "Failed to read header from file" << std::endl;
        return false;
    }

    if (!validateHeader(header)) {
        return false;
    }

    // std::cout << "Loading WorldGraph with " << header.mapCount << " maps..." << std::endl;

    // Se positionner à l'index
    // file.seekg(header.indexOffset);

    // Lire l'index
    std::vector<IndexEntry> index(header.mapCount);
    file.read(reinterpret_cast<char*>(index.data()),
        header.mapCount * sizeof(IndexEntry));

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

        // Stocker dans les structures internes de WorldGraph
        worldGraph.m_mapData[mapData.mapId] = mapData;
        worldGraph.m_mapCoordinates[mapData.mapId] = mapData.position;

        // Créer la liste d'adjacence pour le graphe
        if (!mapData.edges.empty()) {
            worldGraph.m_worldGraph[mapData.mapId] = mapData.edges;
        }
    }

    if (!file.good() && !file.eof()) {
        std::cerr << "Error reading file" << std::endl;
        std::cerr.flush();
        return false;
    }

    // std::cout << "WorldGraph loaded successfully!" << std::endl;
    return true;
}