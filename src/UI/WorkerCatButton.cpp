#include "UI/WorkerCatButton.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include "config.hpp"
#include <cmath>

WorkerCatButton::WorkerCatButton(std::function<void()> onClick)
    : m_OnClick(onClick) {
    
    m_FullImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/WorkerCat/full.png");
    m_HalfImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/WorkerCat/half.png");
    m_DarkImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/WorkerCat/dark.png");

    SetDrawable(m_DarkImage);
    SetZIndex(10.0f);

    auto maskImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_Mask = std::make_shared<Util::GameObject>(maskImg, 11.0f);
    m_Mask->SetVisible(false); // 不再需要遮罩模擬亮度

    m_LevelText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, "Lv. 1");
    m_LevelObj = std::make_shared<Util::GameObject>(m_LevelText, 15.0f);

    m_CostText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, "$100");
    m_CostObj = std::make_shared<Util::GameObject>(m_CostText, 15.0f);

    m_Transform.scale = {m_BaseScale, m_BaseScale};
}

std::vector<std::shared_ptr<Util::GameObject>> WorkerCatButton::GetParts() const {
    return { m_LevelObj, m_CostObj }; // 移除了 m_Mask
}

void WorkerCatButton::Update(float currentMoney, int currentLevel, float nextUpgradeCost) {
    m_Level = currentLevel;
    HandleInput();
    UpdateVisuals(currentMoney, currentLevel, nextUpgradeCost);

    // 同步子物件位置
    glm::vec2 pos = m_Transform.translation;
    glm::vec2 currentSize = GetScaledSize();

    // 等級文字 (上方)
    m_LevelObj->m_Transform.translation = {pos.x, pos.y + currentSize.y / 2.0f + 15.0f};
    
    // 價格文字 (下方)
    m_CostObj->m_Transform.translation = {pos.x, pos.y - currentSize.y / 2.0f - 15.0f};
}

void WorkerCatButton::HandleInput() {
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 btnPos = m_Transform.translation;
        glm::vec2 size = GetScaledSize();

        if (mousePos.x >= btnPos.x - size.x/2 && mousePos.x <= btnPos.x + size.x/2 &&
            mousePos.y >= btnPos.y - size.y/2 && mousePos.y <= btnPos.y + size.y/2) {
            
            if (m_CanAfford && m_Level < 8) {
                m_OnClick();
            }
        }
    }
}

void WorkerCatButton::UpdateVisuals(float currentMoney, int currentLevel, float nextUpgradeCost) {
    m_CanAfford = (currentMoney >= nextUpgradeCost);
    
    m_LevelText->SetText("Lv. " + std::to_string(currentLevel));
    
    if (currentLevel >= 8) {
        m_CostText->SetText("MAX");
        SetDrawable(m_FullImage);
    } else {
        m_CostText->SetText("$" + std::to_string(static_cast<int>(nextUpgradeCost)));
        
        if (m_CanAfford) {
            // 錢夠升級：在全亮與半亮之間閃爍
            bool isFull = (static_cast<int>(Util::Time::GetElapsedTimeMs() / 250) % 2 == 0);
            SetDrawable(isFull ? m_FullImage : m_HalfImage);
        } else {
            // 錢不夠：使用暗色圖
            SetDrawable(m_DarkImage);
        }
    }
}
