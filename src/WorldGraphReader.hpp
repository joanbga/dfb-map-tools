#pragma once

#include <string>
#include <cstdint>

class WorldGraph;

struct IndexEntry {
    uint32_t mapId;
    uint32_t dataOffset;
};

class WorldGraphReader {
public:
    static constexpr uint32_t MAGIC = 0x4D415044; // 'MAPD'
    static constexpr uint32_t SUPPORTED_VERSION = 1;

    struct FileHeader {
        uint32_t magic;
        uint32_t version;
        uint32_t mapCount;
        uint32_t indexOffset;
    };

    // Lit le worldgraph depuis un fichier binaire
    static bool readWorldGraphFromBinary(const std::string& filepath, WorldGraph& worldGraph);

private:
    // Vérifie le header du fichier
    static bool validateHeader(const FileHeader& header);
};