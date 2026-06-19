#include "UIManager.hpp"

#include "UnitFactory.hpp"
#include "UI/CatCannonButton.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

UIManager::UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
                     const std::vector<std::string>& catNames,
                     std::function<int(const std::string&)> getCatLevel,
                     std::function<bool(float)> onSpendMoney,
                     std::function<void()> onUpgradeWorker,
                     std::function<void()> onFireCannon)
    : m_Root(root), m_UnitManager(unitManager), 
      m_GetCatLevel(std::move(getCatLevel)),
      m_OnSpendMoney(std::move(onSpendMoney)),
      m_OnUpgradeWorker(std::move(onUpgradeWorker)),
      m_OnFireCannon(std::move(onFireCannon)) {
    SetupButtons(catNames);
}

void UIManager::SetupButtons(const std::vector<std::string>& catNames) {
    // 預設值
    float moneyX = 500.0f, moneyY = 300.0f;
    float workerX = -550.0f, workerY = -310.0f;
    float cannonX = 550.0f, cannonY = -310.0f;
    float gridX = 0.0f, gridY = -310.0f;
    float paddingX = 100.0f, paddingY = 80.0f;
    int columns = 5;

    // 嘗試從 JSON 載入配置
    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("BattleScene")) {
                auto& scene = layout["BattleScene"];
                if (scene.contains("MoneyText")) {
                    moneyX = scene["MoneyText"]["x"].get<float>();
                    moneyY = scene["MoneyText"]["y"].get<float>();
                }
                if (scene.contains("WorkerButton")) {
                    workerX = scene["WorkerButton"]["x"].get<float>();
                    workerY = scene["WorkerButton"]["y"].get<float>();
                }
                if (scene.contains("CatCannonButton")) {
                    cannonX = scene["CatCannonButton"]["x"].get<float>();
                    cannonY = scene["CatCannonButton"]["y"].get<float>();
                }
                if (scene.contains("UnitGrid")) {
                    gridX = scene["UnitGrid"]["x"].get<float>();
                    gridY = scene["UnitGrid"]["y"].get<float>();
                    paddingX = scene["UnitGrid"]["paddingX"].get<float>();
                    paddingY = scene["UnitGrid"]["paddingY"].get<float>();
                    columns = scene["UnitGrid"]["columns"].get<int>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json: {}", e.what());
        }
    }

    m_MoneyText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 30, "Money: $0 / $0");
    m_MoneyObj = std::make_shared<Util::GameObject>(m_MoneyText, 100.0f);
    m_MoneyObj->m_Transform.translation = {moneyX, moneyY};
    m_Root->AddChild(m_MoneyObj);

    // 工作貓按鈕
    m_WorkerButton = std::make_shared<WorkerCatButton>([this]() {
        m_OnUpgradeWorker();
    });
    m_WorkerButton->m_Transform.translation = {workerX, workerY};
    m_Root->AddChild(m_WorkerButton);
    for (auto& part : m_WorkerButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 貓咪砲按鈕
    m_CannonButton = std::make_shared<CatCannonButton>([this]() {
        m_OnFireCannon();
    });
    m_CannonButton->m_Transform.translation = {cannonX, cannonY};
    m_Root->AddChild(m_CannonButton);
    for (auto& part : m_CannonButton->GetParts()) {
        m_Root->AddChild(part);
    }

    const float startX = gridX - ((columns - 1) * paddingX) / 2.0f;
    const float startY = gridY;

    for (size_t i = 0; i < catNames.size(); ++i) {
        const std::string name = catNames[i];
        if (name.empty()) {
            continue;
        }
        const auto data = UnitFactory::Get(name);

        auto btn = std::make_shared<UnitButton>(data, [this, name, data]() {
            if (!m_OnSpendMoney(data.cost)) {
                return;
            }
            int lvl = m_GetCatLevel ? m_GetCatLevel(name) : 1;
            m_UnitManager->AddUnit(UnitFactory::Create(name, Unit::Team::CAT, lvl));
        });

        const int row = static_cast<int>(i / columns);
        const int col = static_cast<int>(i % columns);
        btn->m_Transform.translation = {startX + col * paddingX, startY + row * paddingY};

        m_Buttons.push_back(btn);
        m_Root->AddChild(btn);

        for (auto& part : btn->GetParts()) {
            m_Root->AddChild(part);
        }
    }
}

void UIManager::Update(float currentMoney, int workerLevel, float nextUpgradeCost, float cannonProgress) {
    for (auto& btn : m_Buttons) {
        btn->Update(currentMoney);
    }
    
    if (m_WorkerButton) {
        m_WorkerButton->Update(currentMoney, workerLevel, nextUpgradeCost);
    }

    if (m_CannonButton) {
        m_CannonButton->Update(cannonProgress);
    }

    int money = static_cast<int>(currentMoney);
    m_MoneyText->SetText("Money: $" + std::to_string(money));
}

void UIManager::ResetAllCooldowns() {
    for (auto& btn : m_Buttons) {
        btn->ResetCooldown();
    }
}
