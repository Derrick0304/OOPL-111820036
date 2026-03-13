#include "UnitFactory.hpp"
#include <vector>
#include <string>

std::unordered_map<std::string, UnitData> UnitFactory::s_Registry;

void UnitFactory::Init() {
    // 註冊基礎貓 (BasicCat)
    s_Registry["BasicCat"] = {
        "BasicCat",
        {100.0f, 150.0f, 60.0f, 20.0f, 0.5f}, 
        "/Units/Cats/BasicCat",
        2, 2,
        50.0f, 2.0f,
        "/Units/Cats/BasicCat/icon.png"
    };

    // 註冊狗狗 (Doge)
    s_Registry["Doge"] = {
        "Doge",
        {150.0f, 80.0f, 60.0f, 10.0f, 1.0f},
        "/Units/Enemies/Doge",
        2, 2,
        0.0f, 0.0f,
        ""
    };

    // 註冊坦克貓 (TankCat)
    s_Registry["TankCat"] = {
        "TankCat",
        {400.0f, 80.0f, 50.0f, 10.0f, 1.0f},
        "/Units/Cats/TankCat",
        3, 5,
        150.0f, 5.0f,
        "/Units/Cats/TankCat/icon.png"
    };

    // 註冊斧頭貓 (AxeCat)
    s_Registry["AxeCat"] = {
        "AxeCat",
        {180.0f, 120.0f, 70.0f, 40.0f, 0.8f},
        "/Units/Cats/AxeCat",
        3, 5,
        200.0f, 3.5f,
        "/Units/Cats/AxeCat/icon.png"
    };

    // 註冊噁心貓 (GrossCat) - 長射程, 高成本
    s_Registry["GrossCat"] = {
        "GrossCat",
        {200.0f, 100.0f, 250.0f, 50.0f, 1.5f},
        "/Units/Cats/GrossCat",
        6, 4,
        400.0f, 6.0f,
        "/Units/Cats/GrossCat/icon.png"
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
