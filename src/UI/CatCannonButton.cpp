#include "UI/CatCannonButton.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include "config.hpp"

CatCannonButton::CatCannonButton(std::function<void()> onClick)
    : m_OnClick(onClick) {
    
    m_FullImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/CatCannon/full.png");
    m_GlowImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/CatCannon/glow.png");
    m_DarkImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/CatCannon/dark.png");

    SetDrawable(m_DarkImage);
    SetZIndex(10.0f);

    // 前景 (亮起的圖片)
    m_ForegroundObj = std::make_shared<Util::GameObject>(m_FullImage, 11.0f);
    
    // 遮罩 (用來遮掉亮起的部分，模擬由下往上變亮)
    auto maskImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_ProgressMask = std::make_shared<Util::GameObject>(maskImg, 12.0f);

    m_Transform.scale = {m_BaseScale, m_BaseScale};
    m_ForegroundObj->m_Transform.scale = m_Transform.scale;
}

std::vector<std::shared_ptr<Util::GameObject>> CatCannonButton::GetParts() const {
    return { m_ForegroundObj, m_ProgressMask };
}

void CatCannonButton::Update(float cooldownProgress) {
    m_Progress = cooldownProgress;
    HandleInput();
    UpdateVisuals(cooldownProgress);

    glm::vec2 pos = m_Transform.translation;
    glm::vec2 size = GetScaledSize();

    m_ForegroundObj->m_Transform.translation = pos;
    m_ForegroundObj->m_Transform.scale = m_Transform.scale;

    // 處理遮罩位移與縮放 (從上往下遮住亮圖)
    // 當 progress 為 0 時，遮罩覆蓋全圖；當 progress 為 1 時，遮罩高度為 0
    float maskHeightFactor = 1.0f - m_Progress;
    m_ProgressMask->m_Transform.scale = {size.x, size.y * maskHeightFactor};
    
    // 遮罩中心點位移：當 maskHeightFactor 為 1 時，中心在 pos.y；
    // 當為 0.5 時，中心應在上方 (pos.y + size.y/4)
    m_ProgressMask->m_Transform.translation = {
        pos.x, 
        pos.y + (size.y / 2.0f) * (1.0f - maskHeightFactor)
    };
    
    // 如果冷卻完了，隱藏遮罩
    m_ProgressMask->SetVisible(m_Progress < 1.0f);
}

void CatCannonButton::HandleInput() {
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 btnPos = m_Transform.translation;
        glm::vec2 size = GetScaledSize();

        if (mousePos.x >= btnPos.x - size.x/2 && mousePos.x <= btnPos.x + size.x/2 &&
            mousePos.y >= btnPos.y - size.y/2 && mousePos.y <= btnPos.y + size.y/2) {
            
            if (m_Progress >= 1.0f) {
                m_OnClick();
            }
        }
    }
}

void CatCannonButton::UpdateVisuals(float progress) {
    if (progress >= 1.0f) {
        // 冷卻完畢：在全亮與發光之間閃爍
        bool isGlow = (static_cast<int>(Util::Time::GetElapsedTimeMs() / 150) % 2 == 0);
        m_ForegroundObj->SetDrawable(isGlow ? m_GlowImage : m_FullImage);
    } else {
        // 冷卻中：前景固定為全亮圖，由遮罩控制顯示範圍
        m_ForegroundObj->SetDrawable(m_FullImage);
    }
}
