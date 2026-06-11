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
            stage.chapterId = item.value("chapterId", 1);
            stage.displayName = item.at("displayName").get<std::string>();
            stage.background = item.at("background").get<std::string>();
            stage.enemyBaseHp = item.value("enemyBaseHp", 1000.0f);
            stage.cost = item.value("cost", 0);
            stage.maxEnemies = item.value("maxEnemies", 10);
            stage.stageLength = item.value("stageLength", 3000.0f);
            stage.xpReward = item.value("xpReward", 0);

            if (item.contains("waves") && item["waves"].is_array()) {
                for (const auto& waveItem : item["waves"]) {
                    WaveData wave;
                    wave.triggerTime = waveItem.value("triggerTime", -1.0f);
                    wave.triggerBaseHpPercentage = waveItem.value("triggerBaseHpPercentage", -1.0f);

                    if (waveItem.contains("spawns") && waveItem["spawns"].is_array()) {
                        for (const auto& spawnItem : waveItem["spawns"]) {
                            SpawnEntry spawn;
                            spawn.unit = spawnItem.at("unit").get<std::string>();
                            spawn.count = spawnItem.value("count", 0);
                            
                            if (spawnItem.contains("interval")) {
                                float val = spawnItem.at("interval").get<float>();
                                spawn.intervalMin = val;
                                spawn.intervalMax = val;
                            } else {
                                spawn.intervalMin = spawnItem.value("intervalMin", 0.0f);
                                spawn.intervalMax = spawnItem.value("intervalMax", 0.0f);
                            }
                            
                            spawn.magnification = spawnItem.value("magnification", 1.0f);
                            spawn.groupCount = spawnItem.value("groupCount", 1);
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
