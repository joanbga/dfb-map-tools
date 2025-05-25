// #include "WorldGraphReader.hpp"
// #include "WorldGraph.hpp"
// #include <fstream>
// #include <iostream>
// #include <vector>
// #include <cstring>

// bool WorldGraphReader::validateHeader(const FileHeader& header) {
//     if (header.magic != MAGIC) {
//         std::cerr << "Invalid magic number: " << std::hex << header.magic
//             << " (expected " << MAGIC << ")" << std::dec << std::endl;
//         return false;
//     }

//     if (header.version != SUPPORTED_VERSION) {
//         std::cerr << "Unsupported version: " << header.version
//             << " (supported: " << SUPPORTED_VERSION << ")" << std::endl;
//         return false;
//     }

//     return true;
// }

// bool WorldGraphReader::readWorldGraphFromBinary(const std::string& filepath, WorldGraph& worldGraph) {
//     // std::cerr << "Attempting to open file: " << filepath << std::endl;
//     std::cerr.flush();

//     // Lire tout le fichier en mémoire pour éviter les problèmes de seekg
//     std::ifstream file(filepath, std::ios::binary | std::ios::ate);
//     if (!file.is_open()) {
//         std::cerr << "Failed to open file: " << filepath << std::endl;
//         std::cerr.flush();
//         return false;
//     }

//     size_t fileSize = file.tellg();
//     file.seekg(0, std::ios::beg);

//     // Lire tout le fichier en mémoire
//     std::vector<uint8_t> buffer(fileSize);
//     file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
//     file.close();

//     // Parser depuis la mémoire
//     size_t pos = 0;

//     // Lire le header
//     if (fileSize < sizeof(FileHeader)) {
//         std::cerr << "File too small for header" << std::endl;
//         return false;
//     }

//     FileHeader header;
//     std::memcpy(&header, buffer.data() + pos, sizeof(FileHeader));
//     pos += sizeof(FileHeader);

//     if (!validateHeader(header)) {
//         return false;
//     }

//     // std::cout << "Loading WorldGraph with " << header.mapCount << " maps..." << std::endl;
//     std::cout.flush();

//     // Lire l'index
//     if (header.indexOffset >= fileSize) {
//         std::cerr << "Invalid index offset" << std::endl;
//         return false;
//     }

//     pos = header.indexOffset;
//     std::vector<IndexEntry> index(header.mapCount);
//     std::memcpy(index.data(), buffer.data() + pos, header.mapCount * sizeof(IndexEntry));

//     // Lire les données de chaque map
//     for (const auto& entry : index) {
//         if (entry.dataOffset >= fileSize) {
//             std::cerr << "Invalid data offset for map " << entry.mapId << std::endl;
//             continue;
//         }

//         pos = entry.dataOffset;

//         WorldGraphMapData mapData;
//         mapData.mapId = entry.mapId;

//         // Lire position
//         mapData.position.x = *reinterpret_cast<const int8_t*>(buffer.data() + pos);
//         pos += sizeof(int8_t);
//         mapData.position.y = *reinterpret_cast<const int8_t*>(buffer.data() + pos);
//         pos += sizeof(int8_t);

//         // Lire nombre d'edges
//         uint16_t edgeCount = *reinterpret_cast<const uint16_t*>(buffer.data() + pos);
//         pos += sizeof(uint16_t);

//         // Lire les edges
//         mapData.edges.reserve(edgeCount);
//         for (uint16_t i = 0; i < edgeCount; ++i) {
//             WorldGraphEdge edge;

//             edge.toMapId = *reinterpret_cast<const uint32_t*>(buffer.data() + pos);
//             pos += sizeof(uint32_t);
//             edge.zoneId = *reinterpret_cast<const uint8_t*>(buffer.data() + pos);
//             pos += sizeof(uint8_t);

//             uint8_t transitionCount = *reinterpret_cast<const uint8_t*>(buffer.data() + pos);
//             pos += sizeof(uint8_t);

//             edge.transitions.reserve(transitionCount);
//             for (uint8_t j = 0; j < transitionCount; ++j) {
//                 WorldGraphEdgeTransition transition;

//                 transition.type = *reinterpret_cast<const uint8_t*>(buffer.data() + pos);
//                 pos += sizeof(uint8_t);
//                 transition.direction = *reinterpret_cast<const uint8_t*>(buffer.data() + pos);
//                 pos += sizeof(uint8_t);
//                 transition.skillId = *reinterpret_cast<const int16_t*>(buffer.data() + pos);
//                 pos += sizeof(int16_t);
//                 transition.transitionMapId = *reinterpret_cast<const uint32_t*>(buffer.data() + pos);
//                 pos += sizeof(uint32_t);
//                 transition.cellId = *reinterpret_cast<const uint16_t*>(buffer.data() + pos);
//                 pos += sizeof(uint16_t);

//                 edge.transitions.push_back(transition);
//             }

//             mapData.edges.push_back(edge);
//         }

//         // Stocker dans les structures internes
//         worldGraph.m_mapData[mapData.mapId] = mapData;
//         worldGraph.m_mapCoordinates[mapData.mapId] = mapData.position;

//         if (!mapData.edges.empty()) {
//             worldGraph.m_worldGraph[mapData.mapId] = mapData.edges;
//         }
//     }

//     // std::cout << "WorldGraph loaded successfully!" << std::endl;
//     std::cout.flush();
//     return true;
// }