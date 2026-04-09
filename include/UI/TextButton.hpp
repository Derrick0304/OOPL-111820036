#ifndef TEXT_BUTTON_HPP
#define TEXT_BUTTON_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

#include <functional>
#include <memory>
#include <string>

class TextButton : public Util::GameObject {
public:
    TextButton(const std::string& label, std::function<void()> onClick, int fontSize = 36);

    void Update();
    void SetLabel(const std::string& label);
    void SetTextColor(const Util::Color& normalColor, const Util::Color& hoverColor);

private:
    bool IsCursorInside() const;

    std::shared_ptr<Util::Text> m_Text;
    std::function<void()> m_OnClick;
    glm::vec2 m_Padding = {20.0f, 12.0f};
    Util::Color m_NormalColor = Util::Color(255, 255, 255);
    Util::Color m_HoverColor = Util::Color(255, 235, 120);
};

#endif
