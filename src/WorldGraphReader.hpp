#pragma once

#include <string>
#include <cstdint>

class WorldGraph;

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

    struct IndexEntry {
        uint32_t mapId;
        uint32_t dataOffset;
    };
};