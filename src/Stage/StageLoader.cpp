#include "Stage/StageLoader.hpp"

#include "Util/Logger.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<StageData> StageLoader::s_Stages;

bool StageLoader::Load() {
    const std::string path = RESOURCE_DIR"/Data/Stages.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open Stages.json at {}", path);
        s_Stages.clear();
        return false;
    }

    try {
        const json data = json::parse(file);
        s_Stages.clear();

        if (!data.contains("stages") || !data["stages"].is_array()) {
            LOG_ERROR("Stages.json missing array field: stages");
            return false;
        }

        for (const auto& item : data["stages"]) {
            StageData stage;
            stage.id = item.at("id").get<std::string>();
            stage.displayName = item.at("displayName").get<std::string>();
            stage.background = item.at("background").get<std::string>();
            stage.enemyBaseHp = item.at("enemyBaseHp").get<float>();

            if (item.contains("waves") && item["waves"].is_array()) {
                for (const auto& waveItem : item["waves"]) {
                    WaveData wave;
                    wave.triggerTime = waveItem.at("triggerTime").get<float>();

                    if (waveItem.contains("spawns") && waveItem["spawns"].is_array()) {
                        for (const auto& spawnItem : waveItem["spawns"]) {
                            SpawnEntry spawn;
                            spawn.unit = spawnItem.at("unit").get<std::string>();
                            spawn.count = spawnItem.at("count").get<int>();
                            spawn.interval = spawnItem.at("interval").get<float>();
                            wave.spawns.push_back(spawn);
                        }
                    }

                    stage.waves.push_back(wave);
                }
            }

            s_Stages.push_back(stage);
        }

        LOG_INFO("Loaded {} stages from JSON", s_Stages.size());
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error parsing Stages.json: {}", e.what());
        s_Stages.clear();
        return false;
    }
}

const std::vector<StageData>& StageLoader::GetAllStages() {
    return s_Stages;
}

const StageData* StageLoader::GetStageById(const std::string& id) {
    const auto it = std::find_if(s_Stages.begin(), s_Stages.end(), [&id](const StageData& stage) {
        return stage.id == id;
    });

    if (it == s_Stages.end()) {
        return nullptr;
    }
    return &(*it);
}
