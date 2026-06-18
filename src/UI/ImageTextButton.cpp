#include "UI/ImageTextButton.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include <cmath>

ImageTextButton::ImageTextButton(const std::string& label, std::function<void()> onClick, Type type)
    : m_OnClick(std::move(onClick)) {
    
    std::string base, yellow, purple;
    if (type == Type::LONG) {
        base = "/UI/Buttons/ButtonLong_Base.png";
        yellow = "/UI/Buttons/ButtonLong_Yellow.png";
        purple = "/UI/Buttons/ButtonLong_Purple.png";
    } else {
        base = "/UI/Buttons/ButtonShort_Base.png";
        yellow = "/UI/Buttons/ButtonShort_Yellow.png";
        purple = "/UI/Buttons/ButtonShort_Purple.png";
    }

    // 底圖
    SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR + base));
    SetZIndex(10.0f);

    // 黃色邊框
    m_YellowBorder = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR + yellow), 11.0f);
    m_YellowBorder->SetVisible(false);

    // 紫色邊框
    m_PurpleBorder = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR + purple), 12.0f);
    m_PurpleBorder->SetVisible(false);

    // 文字
    m_Text = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 24, label, Util::Color(0, 0, 0)); // 預設黑色文字
    m_TextObj = std::make_shared<Util::GameObject>(m_Text, 15.0f);
}

ImageTextButton::ImageTextButton(const std::string& label, std::function<void()> onClick, 
                                 const std::string& customBase, 
                                 const std::string& customYellow, 
                                 const std::string& customPurple)
    : m_OnClick(std::move(onClick)) {
    
    // 底圖
    SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR + customBase));
    SetZIndex(10.0f);

    // 黃色邊框
    if (!customYellow.empty()) {
        m_YellowBorder = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR + customYellow), 11.0f);
    } else {
        m_YellowBorder = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/ButtonLong_Yellow.png"), 11.0f);
    }
    m_YellowBorder->SetVisible(false);

    // 紫色邊框
    if (!customPurple.empty()) {
        m_PurpleBorder = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR + customPurple), 12.0f);
    } else {
        m_PurpleBorder = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/ButtonLong_Purple.png"), 12.0f);
    }
    m_PurpleBorder->SetVisible(false);

    // 文字
    m_Text = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 24, label, Util::Color(0, 0, 0));
    m_TextObj = std::make_shared<Util::GameObject>(m_Text, 15.0f);
}

void ImageTextButton::Update() {
    if (!m_Visible) return;

    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    const bool hovered = IsCursorInside();

    // --- 邊框閃爍動畫 ---
    if (hovered && m_FlashEnabled) {
        m_FlashTimer += dt;
        if (m_FlashTimer > 0.15f) {
            m_FlashTimer = 0.0f;
            m_BorderToggle = !m_BorderToggle;
            m_YellowBorder->SetVisible(m_BorderToggle);
            m_PurpleBorder->SetVisible(!m_BorderToggle);
        }
    } else {
        m_YellowBorder->SetVisible(false);
        m_PurpleBorder->SetVisible(false);
        m_FlashTimer = 0.0f;
    }

    // --- 點擊邏輯與縮放動畫 ---
    if (hovered && Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_ScaleAnimTimer = 0.2f;
        if (m_OnClick) m_OnClick();
    }

    float currentScale = 1.0f;
    if (m_ScaleAnimTimer > 0) {
        m_ScaleAnimTimer -= dt;
        // 點擊時縮小再放大
        currentScale = 1.0f - 0.15f * std::sin((m_ScaleAnimTimer / 0.2f) * 3.14159f);
    }
    m_Transform.scale = {currentScale, currentScale};

    // 同步子物件位置與縮放
    glm::vec2 pos = m_Transform.translation;
    m_YellowBorder->m_Transform.translation = pos;
    m_YellowBorder->m_Transform.scale = m_Transform.scale;
    
    m_PurpleBorder->m_Transform.translation = pos;
    m_PurpleBorder->m_Transform.scale = m_Transform.scale;

    m_TextObj->m_Transform.translation = pos;
    m_TextObj->m_Transform.scale = m_Transform.scale;
}

void ImageTextButton::SetLabel(const std::string& label) {
    m_Text->SetText(label);
}

std::vector<std::shared_ptr<Util::GameObject>> ImageTextButton::GetParts() const {
    return { m_YellowBorder, m_PurpleBorder, m_TextObj };
}

bool ImageTextButton::IsCursorInside() const {
    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const glm::vec2 size = GetScaledSize();
    const glm::vec2 pos = m_Transform.translation;

    return cursor.x >= pos.x - size.x / 2.0f &&
           cursor.x <= pos.x + size.x / 2.0f &&
           cursor.y >= pos.y - size.y / 2.0f &&
           cursor.y <= pos.y + size.y / 2.0f;
}
