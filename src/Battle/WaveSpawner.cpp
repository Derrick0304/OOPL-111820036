#include "Battle/WaveSpawner.hpp"

#include "UnitFactory.hpp"
#include <algorithm>

WaveSpawner::WaveSpawner(const StageData& stageData, UnitManager* unitManager)
    : m_Waves(stageData.waves), m_UnitManager(unitManager), m_MaxEnemies(stageData.maxEnemies), m_Triggered(stageData.waves.size(), false) {
    std::random_device rd;
    m_RandomEngine = std::mt19937(rd());
}

void WaveSpawner::Update(float dt) {
    m_Elapsed += dt;

    // Check triggers for each wave
    for (size_t i = 0; i < m_Waves.size(); ++i) {
        if (!m_Triggered[i]) {
            bool shouldTrigger = false;
            
            if (m_Waves[i].triggerBaseHpPercentage > 0.0f) {
                // Base HP trigger takes precedence if set
                if (m_UnitManager && m_UnitManager->GetEnemyBaseHpPercentage() <= m_Waves[i].triggerBaseHpPercentage) {
                    shouldTrigger = true;
                }
            } else if (m_Waves[i].triggerTime >= 0.0f) {
                // Time trigger
                if (m_Elapsed >= m_Waves[i].triggerTime) {
                    shouldTrigger = true;
                }
            } else {
                // If both are -1 or invalid, trigger immediately (e.g., at time 0)
                if (m_Elapsed >= 0.0f) {
                    shouldTrigger = true;
                }
            }

            if (shouldTrigger) {
                TriggerWave(m_Waves[i]);
                m_Triggered[i] = true;
            }
        }
    }

    // Process active spawns
    for (auto it = m_ActiveSpawns.begin(); it != m_ActiveSpawns.end();) {
        auto& spawn = *it;

        // Check if finished spawning (and not infinite)
        if (spawn.entry.count != -1 && spawn.spawnedCount >= spawn.entry.count) {
            it = m_ActiveSpawns.erase(it);
            continue;
        }

        spawn.timer -= dt;
        
        // Try to spawn if timer is up and limit is not reached
        while ((spawn.entry.count == -1 || spawn.spawnedCount < spawn.entry.count) && spawn.timer <= 0.0f) {
            // Check enemy limit before spawning
            if (m_UnitManager && m_UnitManager->GetEnemyCount() >= m_MaxEnemies) {
                // Limit reached: stop processing this spawn temporarily. 
                // Keep timer at 0 or slightly negative so it spawns immediately when limit allows.
                break; 
            }

            SpawnUnit(spawn.entry.unit, spawn.entry.magnification);
            ++spawn.spawnedCount;
            
            if (spawn.entry.count != -1 && spawn.spawnedCount >= spawn.entry.count) {
                break;
            }
            
            // Set timer for next spawn using random interval
            spawn.timer += GetRandomInterval(spawn.entry.intervalMin, spawn.entry.intervalMax);
        }

        if (spawn.entry.count != -1 && spawn.spawnedCount >= spawn.entry.count) {
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
        activeSpawn.timer = 0.0f; // Spawn the first unit immediately
        m_ActiveSpawns.push_back(activeSpawn);
    }
}

void WaveSpawner::SpawnUnit(const std::string& unitName, float magnification) {
    if (!m_UnitManager) {
        return;
    }

    auto unit = UnitFactory::Create(unitName, Unit::Team::ENEMY);
    if (unit) {
        unit->ApplyMagnification(magnification);
        m_UnitManager->AddUnit(unit);
    }
}

float WaveSpawner::GetRandomInterval(float min, float max) {
    if (min >= max) return std::max(min, 0.01f);
    std::uniform_real_distribution<float> dist(min, max);
    return std::max(dist(m_RandomEngine), 0.01f);
}
