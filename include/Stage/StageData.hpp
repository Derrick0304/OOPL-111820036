#ifndef STAGE_DATA_HPP
#define STAGE_DATA_HPP

#include <string>
#include <vector>

struct SpawnEntry {
    std::string unit;
    int count = 0; // -1 for infinite
    float intervalMin = 0.0f;
    float intervalMax = 0.0f;
    float magnification = 1.0f;
};

struct WaveData {
    float triggerTime = -1.0f; // -1 means ignore time
    float triggerBaseHpPercentage = -1.0f; // -1 means ignore base HP
    std::vector<SpawnEntry> spawns;
};

struct StageData {
    std::string id;
    std::string displayName;
    std::string background;
    int cost = 0;
    int maxEnemies = 10;
    float stageLength = 3000.0f;
    int xpReward = 0;
    float enemyBaseHp = 1000.0f;
    std::vector<WaveData> waves;
};

#endif
