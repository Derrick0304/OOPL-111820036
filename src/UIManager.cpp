#include "UIManager.hpp"

#include "UnitFactory.hpp"

UIManager::UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
                     std::function<bool(float)> onSpendMoney)
    : m_Root(root), m_UnitManager(unitManager), m_OnSpendMoney(std::move(onSpendMoney)) {
    SetupButtons();

    m_MoneyText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 30, "Money: $0");
    m_MoneyObj = std::make_shared<Util::GameObject>(m_MoneyText, 100.0f);
    m_MoneyObj->m_Transform.translation = {500.0f, 300.0f};
    m_Root->AddChild(m_MoneyObj);
}

void UIManager::SetupButtons() {
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

void UIManager::Update(float currentMoney) {
    for (auto& btn : m_Buttons) {
        btn->Update(currentMoney);
    }
    m_MoneyText->SetText("Money: $" + std::to_string(static_cast<int>(currentMoney)));
}
