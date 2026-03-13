#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "UnitButton.hpp"
#include "UnitManager.hpp"
#include <vector>
#include <memory>
#include <functional>

class UIManager {
public:
    // 修改建構子，增加扣錢的回調函式
    UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager, 
              std::function<void(float)> onSpendMoney);

    void Update(float currentMoney);

private:
    void SetupButtons();

    std::shared_ptr<Util::GameObject> m_Root;
    UnitManager* m_UnitManager;
    std::function<void(float)> m_OnSpendMoney;
    std::vector<std::shared_ptr<UnitButton>> m_Buttons;

    std::shared_ptr<Util::Text> m_MoneyText;
    std::shared_ptr<Util::GameObject> m_MoneyObj;
};

#endif
