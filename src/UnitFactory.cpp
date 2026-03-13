#include "UnitFactory.hpp"
#include <vector>
#include <string>

std::unordered_map<std::string, UnitData> UnitFactory::s_Registry;

void UnitFactory::Init() {
    // 註冊基礎貓 (BasicCat)
    s_Registry["BasicCat"] = {
        "BasicCat",
        {100.0f, 150.0f, 60.0f, 20.0f, 0.5f}, // HP, Speed, Range, DMG, Interval
        "/Units/Cats/BasicCat",
        2, // Walk Frames
        2  // Attack Frames
    };

    // 註冊狗狗 (Doge)
    s_Registry["Doge"] = {
        "Doge",
        {150.0f, 80.0f, 60.0f, 10.0f, 1.0f},
        "/Units/Enemies/Doge",
        2,
        2
    };
}

std::shared_ptr<Unit> UnitFactory::Create(const std::string& name, Unit::Team team) {
    if (s_Registry.find(name) == s_Registry.end()) return nullptr;

    const auto& data = s_Registry[name];
    auto walkPaths = GeneratePaths(data.resourcePath, "Walk", data.walkFrames);
    auto attackPaths = GeneratePaths(data.resourcePath, "Attack", data.attackFrames);

    return std::make_shared<Unit>(team, data.stats, walkPaths, attackPaths);
}

std::vector<std::string> UnitFactory::GeneratePaths(const std::string& folder, const std::string& state, int count) {
    std::vector<std::string> paths;
    for (int i = 0; i < count; ++i) {
        paths.push_back(RESOURCE_DIR + folder + "/" + state + "/" + std::to_string(i) + ".png");
    }
    return paths;
}
