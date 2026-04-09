#include "Battle/WaveSpawner.hpp"

#include "UnitFactory.hpp"

WaveSpawner::WaveSpawner(const std::vector<WaveData>& waves, UnitManager* unitManager)
    : m_Waves(waves), m_UnitManager(unitManager), m_Triggered(waves.size(), false) {}

void WaveSpawner::Update(float dt) {
    m_Elapsed += dt;

    for (size_t i = 0; i < m_Waves.size(); ++i) {
        if (!m_Triggered[i] && m_Elapsed >= m_Waves[i].triggerTime) {
            TriggerWave(m_Waves[i]);
            m_Triggered[i] = true;
        }
    }

    for (auto it = m_ActiveSpawns.begin(); it != m_ActiveSpawns.end();) {
        auto& spawn = *it;

        if (spawn.spawnedCount >= spawn.entry.count) {
            it = m_ActiveSpawns.erase(it);
            continue;
        }

        spawn.timer -= dt;
        while (spawn.spawnedCount < spawn.entry.count && spawn.timer <= 0.0f) {
            SpawnUnit(spawn.entry.unit);
            ++spawn.spawnedCount;
            if (spawn.spawnedCount >= spawn.entry.count) {
                break;
            }
            spawn.timer += std::max(spawn.entry.interval, 0.01f);
        }

        if (spawn.spawnedCount >= spawn.entry.count) {
            it = m_ActiveSpawns.erase(it);
        } else {
            ++it;
        }
    }
}

void WaveSpawner::Reset() {
    m_Elapsed = 0.0f;
    m_Triggered.assign(m_Waves.size(), false);
    m_ActiveSpawns.clear();
}

bool WaveSpawner::IsFinished() const {
    return std::all_of(m_Triggered.begin(), m_Triggered.end(), [](bool triggered) {
               return triggered;
           }) &&
           m_ActiveSpawns.empty();
}

void WaveSpawner::TriggerWave(const WaveData& wave) {
    for (const auto& spawn : wave.spawns) {
        ActiveSpawn activeSpawn;
        activeSpawn.entry = spawn;
        activeSpawn.timer = 0.0f;
        m_ActiveSpawns.push_back(activeSpawn);
    }
}

void WaveSpawner::SpawnUnit(const std::string& unitName) {
    if (!m_UnitManager) {
        return;
    }

    m_UnitManager->AddUnit(UnitFactory::Create(unitName, Unit::Team::ENEMY));
}
