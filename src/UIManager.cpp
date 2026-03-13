#include "UIManager.hpp"
#include "UnitFactory.hpp"
#include "Util/Logger.hpp"

UIManager::UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
                    std::function<void(float)> onSpendMoney)
    : m_Root(root), m_UnitManager(unitManager), m_OnSpendMoney(onSpendMoney) {
    
    SetupButtons();

    m_MoneyText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 30, "Money: $0");
    m_MoneyObj = std::make_shared<Util::GameObject>(m_MoneyText, 100.0f);
    m_MoneyObj->m_Transform.translation = {500.0f, 300.0f};
    m_Root->AddChild(m_MoneyObj);
}
void UIManager::SetupButtons() {
    // 註冊目前已有的貓咪種類，加入 CowCat, BirdCat，其餘用 BasicCat 填補
    std::vector<std::string> catNames = {
        "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat",
        "BirdCat", "BasicCat", "BasicCat", "BasicCat", "BasicCat"
    };


    float paddingX = 100.0f; // 按鈕更小，間距再縮小
    float paddingY = 80.0f;
    float startX = -((5 - 1) * paddingX) / 2.0f;
    float startY = -310.0f; // 從 -280.0f 下移至 -310.0f

    for (size_t i = 0; i < catNames.size(); ++i) {
        std::string name = catNames[i];
        auto data = UnitFactory::Get(name);

        auto btn = std::make_shared<UnitButton>(data, [this, name, data]() {
            m_UnitManager->AddUnit(UnitFactory::Create(name, Unit::Team::CAT));
            m_OnSpendMoney(data.cost);
        });

        int row = i / 5; 
        int col = i % 5;
        btn->m_Transform.translation = {startX + col * paddingX, startY + row * paddingY};

        m_Buttons.push_back(btn);
        m_Root->AddChild(btn);

        // 關鍵：將零件也加入 root 才能被繪製
        for (auto& part : btn->GetParts()) {
            m_Root->AddChild(part);
        }
    }
}

void UIManager::Update(float currentMoney) {
    for (auto& btn : m_Buttons) {
        btn->Update(currentMoney);
    }
    m_MoneyText->SetText("Money: $" + std::to_string((int)currentMoney));
}
