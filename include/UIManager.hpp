#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "UnitButton.hpp"
#include "UI/WorkerCatButton.hpp"
#include "UI/CatCannonButton.hpp"
#include "UnitManager.hpp"

#include <functional>
#include <memory>
#include <vector>

class UIManager {
public:
    UIManager(std::shared_ptr<Util::GameObject> root, UnitManager* unitManager,
              std::function<bool(float)> onSpendMoney,
              std::function<void()> onUpgradeWorker,
              std::function<void()> onFireCannon);

    void Update(float currentMoney, int workerLevel, float nextUpgradeCost, float cannonProgress);

private:
    void SetupButtons();

    std::shared_ptr<Util::GameObject> m_Root;
    UnitManager* m_UnitManager;
    std::function<bool(float)> m_OnSpendMoney;
    std::function<void()> m_OnUpgradeWorker;
    std::function<void()> m_OnFireCannon;
    
    std::vector<std::shared_ptr<UnitButton>> m_Buttons;
    std::shared_ptr<WorkerCatButton> m_WorkerButton;
    std::shared_ptr<CatCannonButton> m_CannonButton;

    std::shared_ptr<Util::Text> m_MoneyText;
    std::shared_ptr<Util::GameObject> m_MoneyObj;
};

#endif
