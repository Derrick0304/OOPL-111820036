#ifndef CAT_CANNON_EFFECT_HPP
#define CAT_CANNON_EFFECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <vector>

class CatCannonEffect {
public:
    CatCannonEffect(std::shared_ptr<Util::GameObject> root);

    void Trigger(float startX, float endX, float y);
    void Update(float dt);
    bool IsActive() const { return m_IsActive; }

private:
    std::shared_ptr<Util::GameObject> m_Root;
    std::shared_ptr<Util::GameObject> m_Laser;
    std::vector<std::shared_ptr<Util::Image>> m_ExplosionFrames;
    std::vector<std::shared_ptr<Util::GameObject>> m_Explosions;

    bool m_IsActive = false;
    float m_Timer = 0.0f;
    float m_StartX, m_EndX, m_Y;

    const float LASER_DURATION = 0.5f;
    const float EXPLOSION_DELAY = 0.4f;
    const float TOTAL_DURATION = 1.5f;
};

#endif
