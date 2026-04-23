#include "Battle/CatCannonEffect.hpp"
#include "Util/Time.hpp"
#include "config.hpp"
#include <cmath>

CatCannonEffect::CatCannonEffect(std::shared_ptr<Util::GameObject> root) : m_Root(root) {
    // 激光使用紫色像素圖片
    auto laserImg = std::make_shared<Util::Image>(RESOURCE_DIR"/Effects/CatCannon/purple_pixel.png");
    m_Laser = std::make_shared<Util::GameObject>(laserImg, 50.0f);
    m_Laser->SetVisible(false);
    m_Root->AddChild(m_Laser);

    // 載入爆炸序列幀
    m_ExplosionFrames.push_back(std::make_shared<Util::Image>(RESOURCE_DIR"/Effects/CatCannon/exp_0.png"));
    m_ExplosionFrames.push_back(std::make_shared<Util::Image>(RESOURCE_DIR"/Effects/CatCannon/exp_1.png"));
    m_ExplosionFrames.push_back(std::make_shared<Util::Image>(RESOURCE_DIR"/Effects/CatCannon/exp_2.png"));
    m_ExplosionFrames.push_back(std::make_shared<Util::Image>(RESOURCE_DIR"/Effects/CatCannon/exp_3.png"));

    // 預先建立 10 個爆炸點
    for (int i = 0; i < 10; ++i) {
        auto exp = std::make_shared<Util::GameObject>(m_ExplosionFrames[0], 51.0f);
        exp->SetVisible(false);
        m_Explosions.push_back(exp);
        m_Root->AddChild(exp);
    }
}

void CatCannonEffect::Trigger(float startX, float endX, float y) {
    m_IsActive = true;
    m_Timer = 0.0f;
    m_StartX = startX;
    m_EndX = endX;
    m_Y = y;
}

void CatCannonEffect::Update(float dt) {
    if (!m_IsActive) return;

    m_Timer += dt;

    // 1. 激光演出 (從塔頂射向地面的紫色斜線)
    if (m_Timer < LASER_DURATION) {
        m_Laser->SetVisible(true);
        float progress = m_Timer / LASER_DURATION;
        
        // 起點：我方基地上方 (固定)
        glm::vec2 origin = {m_StartX, 100.0f}; 
        // 終點：隨時間橫掃地面的目標點
        glm::vec2 target = {m_StartX + (m_EndX - m_StartX) * progress, m_Y};
        
        // 計算兩點間的中間位置、距離與角度
        glm::vec2 diff = target - origin;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        float angle = std::atan2(diff.y, diff.x);
        
        m_Laser->m_Transform.translation = (origin + target) / 2.0f;
        m_Laser->m_Transform.rotation = angle;
        
        // X 軸縮放代表長度，Y 軸縮放代表粗細 (加上抖動)
        float thickness = 10.0f + std::sin(m_Timer * 40.0f) * 4.0f;
        m_Laser->m_Transform.scale = {distance, thickness};
    } else {
        m_Laser->SetVisible(false);
    }

    // 2. 爆炸序列演出
    if (m_Timer > EXPLOSION_DELAY) {
        float explosionTotalLife = 0.5f; // 單個爆炸點持續 0.5 秒
        
        for (size_t i = 0; i < m_Explosions.size(); ++i) {
            // 每個爆炸點依序啟動 (0.5 秒內觸發完 10 個)
            float triggerTime = (static_cast<float>(i) / m_Explosions.size()) * 0.5f;
            float lifeTime = (m_Timer - EXPLOSION_DELAY) - triggerTime;

            if (lifeTime > 0 && lifeTime < explosionTotalLife) {
                m_Explosions[i]->SetVisible(true);
                float posX = m_StartX + (m_EndX - m_StartX) * (static_cast<float>(i) / m_Explosions.size());
                m_Explosions[i]->m_Transform.translation = {posX, m_Y};
                
                // 計算當前應該顯示哪一幀 (4 幀平分 0.5 秒)
                int frameIdx = static_cast<int>((lifeTime / explosionTotalLife) * m_ExplosionFrames.size());
                frameIdx = std::min(frameIdx, static_cast<int>(m_ExplosionFrames.size() - 1));
                
                m_Explosions[i]->SetDrawable(m_ExplosionFrames[frameIdx]);
                
                // 基礎縮放 (根據圖片性質，您可能需要調整 scale)
                m_Explosions[i]->m_Transform.scale = {2.0f, 2.0f}; 
            } else {
                m_Explosions[i]->SetVisible(false);
            }
        }
    }

    if (m_Timer > TOTAL_DURATION) {
        m_IsActive = false;
        m_Laser->SetVisible(false);
        for (auto& e : m_Explosions) e->SetVisible(false);
    }
}
