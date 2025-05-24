#include "WorldGraph.hpp"
#include "WorldGraphReader.hpp"
#include <algorithm>

bool WorldGraph::loadFromBinary(const std::string& filepath) {
    return WorldGraphReader::readWorldGraphFromBinary(filepath, *this);
}

const WorldGraphMapData* WorldGraph::findMap(uint32_t mapId) const {
    auto it = m_mapData.find(mapId);
    if (it != m_mapData.end()) {
        return &it->second;
    }
    return nullptr;
}

Vec2 WorldGraph::getMapCoordinates(uint32_t mapId) const {
    auto it = m_mapCoordinates.find(mapId);
    if (it != m_mapCoordinates.end()) {
        return it->second;
    }
    return { 0, 0 };
}

const std::vector<WorldGraphEdge>* WorldGraph::getMapEdges(uint32_t mapId) const {
    auto it = m_worldGraph.find(mapId);
    if (it != m_worldGraph.end()) {
        return &it->second;
    }
    return nullptr;
}

bool WorldGraph::mapExists(uint32_t mapId) const {
    return m_mapData.find(mapId) != m_mapData.end();
}

std::vector<uint32_t> WorldGraph::getAllMapIds() const {
    std::vector<uint32_t> mapIds;
    mapIds.reserve(m_mapData.size());

    for (const auto& pair : m_mapData) {
        mapIds.push_back(pair.first);
    }

    std::sort(mapIds.begin(), mapIds.end());
    return mapIds;
}

size_t WorldGraph::getTotalEdgeCount() const {
    size_t count = 0;
    for (const auto& pair : m_worldGraph) {
        count += pair.second.size();
    }
    return count;
}

size_t WorldGraph::getTotalTransitionCount() const {
    size_t count = 0;
    for (const auto& pair : m_worldGraph) {
        for (const auto& edge : pair.second) {
            count += edge.transitions.size();
        }
    }
    return count;
}