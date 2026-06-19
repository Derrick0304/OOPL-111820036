#ifndef IMAGE_TEXT_BUTTON_HPP
#define IMAGE_TEXT_BUTTON_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/Color.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class ImageTextButton : public Util::GameObject {
public:
    enum class Type {
        LONG,
        SHORT
    };

    ImageTextButton(const std::string& label, std::function<void()> onClick, Type type = Type::LONG);
    ImageTextButton(const std::string& label, std::function<void()> onClick, 
                    const std::string& customBase, 
                    const std::string& customYellow = "", 
                    const std::string& customPurple = "");

    void Update();
    void SetLabel(const std::string& label);
    
    // 為了讓 Renderer 能渲染所有零件
    std::vector<std::shared_ptr<Util::GameObject>> GetParts() const;

    void SetFlashEnabled(bool enabled) { m_FlashEnabled = enabled; }
    void SetBaseScale(float scale) { m_BaseScale = scale; }

    void SetZIndex(float zIndex) {
        Util::GameObject::SetZIndex(zIndex);
        if (m_YellowBorder) m_YellowBorder->SetZIndex(zIndex + 1.0f);
        if (m_PurpleBorder) m_PurpleBorder->SetZIndex(zIndex + 2.0f);
        if (m_TextObj) m_TextObj->SetZIndex(zIndex + 5.0f);
    }

private:
    bool IsCursorInside() const;

    std::shared_ptr<Util::Text> m_Text;
    std::shared_ptr<Util::GameObject> m_TextObj;
    std::shared_ptr<Util::GameObject> m_YellowBorder;
    std::shared_ptr<Util::GameObject> m_PurpleBorder;
    
    std::function<void()> m_OnClick;
    
    float m_FlashTimer = 0.0f;
    float m_ScaleAnimTimer = 0.0f;
    float m_BaseScale = 1.0f;
    bool m_IsPressed = false;
    bool m_BorderToggle = true;
    bool m_FlashEnabled = true;
};

#endif
