#ifndef WORKER_CAT_BUTTON_HPP
#define WORKER_CAT_BUTTON_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include <functional>
#include <vector>

class WorkerCatButton : public Util::GameObject {
public:
    WorkerCatButton(std::function<void()> onClick);

    void Update(float currentMoney, int currentLevel, float nextUpgradeCost);
    
    std::vector<std::shared_ptr<Util::GameObject>> GetParts() const;

private:
    void HandleInput();
    void UpdateVisuals(float currentMoney, int currentLevel, float nextUpgradeCost);

    std::function<void()> m_OnClick;
    bool m_CanAfford = false;
    int m_Level = 1;

    std::shared_ptr<Util::GameObject> m_Mask; // 保留 mask 用於其他效果或移除
    std::shared_ptr<Util::Image> m_FullImage;
    std::shared_ptr<Util::Image> m_HalfImage;
    std::shared_ptr<Util::Image> m_DarkImage;

    std::shared_ptr<Util::GameObject> m_LevelObj;
    std::shared_ptr<Util::Text> m_LevelText;
    std::shared_ptr<Util::GameObject> m_CostObj;
    std::shared_ptr<Util::Text> m_CostText;

    float m_BaseScale = 0.8f;
};

#endif
