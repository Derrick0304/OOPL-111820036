#ifndef STAGE_DATA_HPP
#define STAGE_DATA_HPP

#include <string>
#include <vector>

struct SpawnEntry {
    std::string unit;
    int count = 0;
    float interval = 0.0f;
};

struct WaveData {
    float triggerTime = 0.0f;
    std::vector<SpawnEntry> spawns;
};

struct StageData {
    std::string id;
    std::string displayName;
    std::string background;
    float enemyBaseHp = 1000.0f;
    std::vector<WaveData> waves;
};

#endif
