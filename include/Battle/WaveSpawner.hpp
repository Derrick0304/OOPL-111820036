#ifndef WAVE_SPAWNER_HPP
#define WAVE_SPAWNER_HPP

#include "Stage/StageData.hpp"
#include "UnitManager.hpp"

#include <vector>

class WaveSpawner {
public:
    WaveSpawner(const std::vector<WaveData>& waves, UnitManager* unitManager);

    void Update(float dt);
    void Reset();
    bool IsFinished() const;

private:
    struct ActiveSpawn {
        SpawnEntry entry;
        int spawnedCount = 0;
        float timer = 0.0f;
    };

    void TriggerWave(const WaveData& wave);
    void SpawnUnit(const std::string& unitName);

    std::vector<WaveData> m_Waves;
    UnitManager* m_UnitManager = nullptr;
    std::vector<bool> m_Triggered;
    std::vector<ActiveSpawn> m_ActiveSpawns;
    float m_Elapsed = 0.0f;
};

#endif
