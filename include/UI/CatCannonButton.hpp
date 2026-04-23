#ifndef CAT_CANNON_BUTTON_HPP
#define CAT_CANNON_BUTTON_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <functional>
#include <memory>
#include <vector>

class CatCannonButton : public Util::GameObject {
public:
    CatCannonButton(std::function<void()> onClick);

    void Update(float cooldownProgress); // 0.0 to 1.0
    
    std::vector<std::shared_ptr<Util::GameObject>> GetParts() const;

private:
    void HandleInput();
    void UpdateVisuals(float progress);

    std::function<void()> m_OnClick;
    float m_Progress = 0.0f;

    std::shared_ptr<Util::Image> m_FullImage;
    std::shared_ptr<Util::Image> m_GlowImage;
    std::shared_ptr<Util::Image> m_DarkImage;

    // 用於冷卻條效果
    std::shared_ptr<Util::GameObject> m_ForegroundObj; 
    std::shared_ptr<Util::GameObject> m_ProgressMask;

    float m_BaseScale = 0.8f;
};

#endif
