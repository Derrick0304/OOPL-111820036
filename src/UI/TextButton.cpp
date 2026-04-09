#include "UI/TextButton.hpp"

#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

TextButton::TextButton(const std::string& label, std::function<void()> onClick, int fontSize)
    : m_Text(std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", fontSize, label, Util::Color(255, 255, 255))),
      m_OnClick(std::move(onClick)) {
    SetDrawable(m_Text);
    SetZIndex(20.0f);
}

void TextButton::Update() {
    if (!m_Visible) {
        return;
    }

    const bool hovered = IsCursorInside();
    m_Text->SetColor(hovered ? m_HoverColor : m_NormalColor);

    if (hovered && Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB) && m_OnClick) {
        m_OnClick();
    }
}

void TextButton::SetLabel(const std::string& label) {
    m_Text->SetText(label);
}

void TextButton::SetTextColor(const Util::Color& normalColor, const Util::Color& hoverColor) {
    m_NormalColor = normalColor;
    m_HoverColor = hoverColor;
    m_Text->SetColor(m_NormalColor);
}

bool TextButton::IsCursorInside() const {
    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const glm::vec2 size = GetScaledSize() + (m_Padding * m_Transform.scale);
    const glm::vec2 pos = m_Transform.translation;

    return cursor.x >= pos.x - size.x / 2.0f &&
           cursor.x <= pos.x + size.x / 2.0f &&
           cursor.y >= pos.y - size.y / 2.0f &&
           cursor.y <= pos.y + size.y / 2.0f;
}
