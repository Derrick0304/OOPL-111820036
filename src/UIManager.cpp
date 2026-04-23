#include "UIManager.hpp"

#include "UnitFactory.hpp"
#include "UI/CatCannonButton.hpp"

UIManager::UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
                     std::function<bool(float)> onSpendMoney,
                     std::function<void()> onUpgradeWorker,
                     std::function<void()> onFireCannon)
    : m_Root(root), m_UnitManager(unitManager), 
      m_OnSpendMoney(std::move(onSpendMoney)),
      m_OnUpgradeWorker(std::move(onUpgradeWorker)),
      m_OnFireCannon(std::move(onFireCannon)) {
    SetupButtons();

    m_MoneyText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 30, "Money: $0 / $0");
    m_MoneyObj = std::make_shared<Util::GameObject>(m_MoneyText, 100.0f);
    m_MoneyObj->m_Transform.translation = {500.0f, 300.0f};
    m_Root->AddChild(m_MoneyObj);
}

void UIManager::SetupButtons() {
    // 工作貓按鈕 - 放在左下角
    m_WorkerButton = std::make_shared<WorkerCatButton>([this]() {
        m_OnUpgradeWorker();
    });
    m_WorkerButton->m_Transform.translation = {-550.0f, -310.0f};
    m_Root->AddChild(m_WorkerButton);
    for (auto& part : m_WorkerButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 貓咪砲按鈕 - 放在右下角
    m_CannonButton = std::make_shared<CatCannonButton>([this]() {
        m_OnFireCannon();
    });
    m_CannonButton->m_Transform.translation = {550.0f, -310.0f};
    m_Root->AddChild(m_CannonButton);
    for (auto& part : m_CannonButton->GetParts()) {
        m_Root->AddChild(part);
    }

    const std::vector<std::string> catNames = {
        "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat",
        "BirdCat", "FishCat", "LizardCat", "TitanCat", "KillerCat"
    };

    const float paddingX = 100.0f;
    const float paddingY = 80.0f;
    const float startX = -((5 - 1) * paddingX) / 2.0f;
    const float startY = -310.0f;

    for (size_t i = 0; i < catNames.size(); ++i) {
        const std::string name = catNames[i];
        const auto data = UnitFactory::Get(name);

        auto btn = std::make_shared<UnitButton>(data, [this, name, data]() {
            if (!m_OnSpendMoney(data.cost)) {
                return;
            }
            m_UnitManager->AddUnit(UnitFactory::Create(name, Unit::Team::CAT));
        });

        const int row = static_cast<int>(i / 5);
        const int col = static_cast<int>(i % 5);
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

    // 取得當前金錢上限 (這裡我們從 BattleScene 的邏輯中得知 UIManager 並沒有直接存取 MaxMoney)
    // 為了精確顯示，通常建議在 Update 參數中加入 MaxMoney
    // 這裡我們先顯示 Money 即可，若需顯示上限，建議修改 Update 簽章
    int money = static_cast<int>(currentMoney);
    m_MoneyText->SetText("Money: $" + std::to_string(money));
}
