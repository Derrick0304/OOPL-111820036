#include "UnitFactory.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Util/Logger.hpp"

using json = nlohmann::json;

std::unordered_map<std::string, UnitData> UnitFactory::s_Registry;

void UnitFactory::Init() {
    std::string path = RESOURCE_DIR"/Data/Units.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open Units.json at {}", path);
        return;
    }

    try {
        json data = json::parse(file);
        
        auto registerGroup = [](const json& group) {
            for (const auto& item : group) {
                UnitData unit;
                unit.name = item["name"];
                
                // 嚴格按照 Unit::Stats 結構順序進行顯式初始化
                unit.stats.maxHp = item["stats"]["hp"];
                unit.stats.speed = item["stats"]["speed"];
                unit.stats.attackRange = item["stats"]["range"];
                unit.stats.attackDamage = item["stats"]["atk"];
                unit.stats.attackInterval = item["stats"]["atkInterval"];

                unit.resourcePath = item["resourcePath"];
                unit.walkFrames = item["walkFrames"];
                unit.attackFrames = item["attackFrames"];
                unit.cost = item["cost"];
                unit.cooldown = item["cooldown"];
                unit.yOffset = item["yOffset"];
                unit.iconPath = item["iconPath"];
                
                s_Registry[unit.name] = unit;
            }
        };

        if (data.contains("Cats")) registerGroup(data["Cats"]);
        if (data.contains("Enemies")) registerGroup(data["Enemies"]);

        LOG_INFO("Successfully loaded {} units from JSON", s_Registry.size());

    } catch (const std::exception& e) {
        LOG_ERROR("Error parsing Units.json: {}", e.what());
    }
}


std::shared_ptr<Unit> UnitFactory::Create(const std::string& name, Unit::Team team) {
    if (s_Registry.find(name) == s_Registry.end()) return nullptr;

    const auto& data = s_Registry[name];
    auto walkPaths = GeneratePaths(data.resourcePath, "Walk", data.walkFrames);
    auto attackPaths = GeneratePaths(data.resourcePath, "Attack", data.attackFrames);

    return std::make_shared<Unit>(team, data.stats, walkPaths, attackPaths, data.yOffset);
}

std::vector<std::string> UnitFactory::GeneratePaths(const std::string& folder, const std::string& state, int count) {
    std::vector<std::string> paths;
    for (int i = 0; i < count; ++i) {
        paths.push_back(RESOURCE_DIR + folder + "/" + state + "/" + std::to_string(i) + ".png");
    }
    return paths;
}
