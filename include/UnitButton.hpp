#ifndef UNIT_BUTTON_HPP
#define UNIT_BUTTON_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UnitData.hpp"
#include <functional>

class UnitButton : public Util::GameObject {
public:
    UnitButton(const UnitData& data, std::function<void()> onClick);

    void Update(float currentMoney);
    
    // 提供給 UIManager 以便加入 Renderer
    std::vector<std::shared_ptr<Util::GameObject>> GetParts() const;

private:
    void HandleInput();
    void UpdateVisuals(float currentMoney);

    UnitData m_Data;
    std::function<void()> m_OnClick;

    float m_CooldownTimer = 0.0f;
    bool m_IsMoneyEnough = false;

    // UI 零件 (獨立物件以確保正確位移)
    std::shared_ptr<Util::GameObject> m_Mask;
    std::shared_ptr<Util::GameObject> m_BlueBar;
    std::shared_ptr<Util::GameObject> m_PriceObj;
    std::shared_ptr<Util::Text> m_PriceText;

    float m_ScaleTimer = 0.0f;
    float m_BaseScale = 0.7f; // 從 0.85f 調小至 0.7f
};

#endif
