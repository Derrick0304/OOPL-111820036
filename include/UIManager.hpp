#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "UnitButton.hpp"
#include "UnitManager.hpp"

#include <functional>
#include <memory>
#include <vector>

class UIManager {
public:
    UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
              std::function<bool(float)> onSpendMoney);

    void Update(float currentMoney);

private:
    void SetupButtons();

    std::shared_ptr<Util::GameObject> m_Root;
    UnitManager* m_UnitManager;
    std::function<bool(float)> m_OnSpendMoney;
    std::vector<std::shared_ptr<UnitButton>> m_Buttons;

    std::shared_ptr<Util::Text> m_MoneyText;
    std::shared_ptr<Util::GameObject> m_MoneyObj;
};

#endif
