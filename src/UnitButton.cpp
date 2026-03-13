#include "UnitButton.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include <cmath>

UnitButton::UnitButton(const UnitData& data, std::function<void()> onClick)
    : m_Data(data), m_OnClick(onClick) {
    
    auto iconImg = std::make_shared<Util::Image>(RESOURCE_DIR + data.iconPath);
    SetDrawable(iconImg);
    SetZIndex(10.0f);

    auto maskImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_Mask = std::make_shared<Util::GameObject>(maskImg, 11.0f);
    m_Mask->SetVisible(false);

    auto barImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlueBar.png");
    m_BlueBar = std::make_shared<Util::GameObject>(barImg, 12.0f);
    m_BlueBar->SetVisible(false);

    m_PriceText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, 
                                             "$" + std::to_string((int)data.cost));
    m_PriceObj = std::make_shared<Util::GameObject>(m_PriceText, 15.0f);

    m_Transform.scale = {m_BaseScale, m_BaseScale};
}

std::vector<std::shared_ptr<Util::GameObject>> UnitButton::GetParts() const {
    return { m_Mask, m_BlueBar, m_PriceObj };
}

void UnitButton::Update(float currentMoney) {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    if (m_CooldownTimer > 0) m_CooldownTimer -= dt;

    HandleInput();
    UpdateVisuals(currentMoney);

    // 處理縮放動畫與基礎縮放的結合
    float animScale = 1.0f;
    if (m_ScaleTimer > 0) {
        m_ScaleTimer -= dt;
        animScale = 1.0f + 0.1f * std::sin(m_ScaleTimer * 20.0f);
    }
    m_Transform.scale = {m_BaseScale * animScale, m_BaseScale * animScale};

    // --- 同步子物件位置與縮放 ---
    glm::vec2 pos = m_Transform.translation;
    glm::vec2 baseSize = GetScaledSize() / animScale; // 原始縮放後的尺寸

    // 1. 遮罩覆蓋全圖
    m_Mask->m_Transform.translation = pos;
    m_Mask->m_Transform.scale = GetScaledSize();

    // 2. 藍色讀條 (位於底部)
    m_BlueBar->m_Transform.translation = {pos.x, pos.y - GetScaledSize().y / 2.0f};
    // 寬度會由 UpdateVisuals 再次修正比例

    // 3. 價格文字 (右上角)
    m_PriceObj->m_Transform.translation = {
        pos.x + GetScaledSize().x / 2.0f - 25.0f * m_BaseScale, 
        pos.y + GetScaledSize().y / 2.0f - 15.0f * m_BaseScale
    };
    m_PriceObj->m_Transform.scale = m_Transform.scale;
}

void UnitButton::HandleInput() {
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 btnPos = m_Transform.translation;
        glm::vec2 size = GetScaledSize();

        if (mousePos.x >= btnPos.x - size.x/2 && mousePos.x <= btnPos.x + size.x/2 &&
            mousePos.y >= btnPos.y - size.y/2 && mousePos.y <= btnPos.y + size.y/2) {
            
            if (m_CooldownTimer <= 0 && m_IsMoneyEnough) {
                m_OnClick();
                m_CooldownTimer = m_Data.cooldown;
                m_ScaleTimer = 0.3f;
            }
        }
    }
}

void UnitButton::UpdateVisuals(float currentMoney) {
    m_IsMoneyEnough = (currentMoney >= m_Data.cost);
    m_Mask->SetVisible(!m_IsMoneyEnough || m_CooldownTimer > 0);
    
    if (m_CooldownTimer > 0) {
        float progress = m_CooldownTimer / m_Data.cooldown;
        m_BlueBar->SetVisible(true);
        m_BlueBar->m_Transform.scale.x = GetScaledSize().x * progress;
        m_BlueBar->m_Transform.scale.y = 5.0f * m_BaseScale;
    } else {
        m_BlueBar->SetVisible(false);
    }
}
