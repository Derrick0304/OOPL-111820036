#include "Unit.hpp"
#include "Util/Time.hpp"

Unit::Unit(Team team, const Stats &stats, 
           const std::vector<std::string> &walkPaths, 
           const std::vector<std::string> &attackPaths,
           float yOffset)
    : m_Team(team), m_Stats(stats), m_YOffset(yOffset), m_CurrentHP(stats.maxHp) {
    
    m_WalkAnim = std::make_shared<Util::Animation>(walkPaths, true, 200, true);
    m_AttackAnim = std::make_shared<Util::Animation>(attackPaths, false, 150, false); // 改為 false (不循環)
    
    m_State = State::WALK;
    SetDrawable(m_WalkAnim);
    m_WalkAnim->Play();

    // --- 設定底部對齊基準點 ---
    // 初始設定，Update 中會動態更新
    m_Pivot = {0, 0}; 
    
    // 縮小為原來的三分之一
    m_Transform.scale = {1.0f, 1.0f};
    
    // 統一地平線座標
    m_Transform.translation.x = (m_Team == Team::CAT) ? 400.0f : -400.0f;
    m_Transform.translation.y = -150.0f; // 這裡的 -150 就是腳底所在的 Y 軸座標
    
    m_ZIndex = 1.0f;
}

void Unit::TriggerAttackAnimation() {
    if (m_State != State::ATTACK) {
        SetState(State::ATTACK);
    }
}

void Unit::SetState(State state) {
    if (m_State == state) return; 
    
    // 如果想要從 ATTACK 切換到 WALK，必須等到動畫結束
    if (m_State == State::ATTACK && state == State::WALK) {
        if (!IsAttackAnimationEnded()) return; // 沒播完，拒絕切換
    }

    m_State = state;
    if (m_State == State::WALK) {
        SetDrawable(m_WalkAnim);
        m_WalkAnim->Play();
    } else if (m_State == State::ATTACK) {
        SetDrawable(m_AttackAnim);
        m_AttackAnim->SetCurrentFrame(0);
        m_AttackAnim->Play();
    }
}

bool Unit::IsAttackAnimationEnded() const {
    // Util::Animation::GetState() 會在一次性動畫播完後返回 ENDED
    return m_AttackAnim->GetState() == Util::Animation::State::ENDED;
}

void Unit::Update() {
    // --- 動態更新 Pivot 以確保對齊 (並加入 yOffset 偏移) ---
    if (m_Drawable) {
        glm::vec2 currentSize = m_Drawable->GetSize();
        
        // 核心邏輯：根據陣營鎖定邊緣，防止因圖片寬度不同而跳動
        float pivotX = 0.0f;
        if (m_Team == Team::CAT) {
            // 貓咪面向左 (x 減少)，鎖定圖片右邊緣 (身體後方)
            // PTSD 引擎中，正的 Pivot.x 會將圖片往負方向 (左) 推
            pivotX = currentSize.x / 2.0f;
        } else {
            // 敵人面向右 (x 增加)，鎖定圖片左邊緣 (身體後方)
            pivotX = -currentSize.x / 2.0f;
        }

        // y 軸維持底部對齊
        m_Pivot = {pivotX, -currentSize.y / 2.0f - m_YOffset};
    }

    // --- 自動循環攻擊動畫 ---
    if (m_State == State::ATTACK) {
        if (IsAttackAnimationEnded()) {
            m_AttackAnim->SetCurrentFrame(0);
            m_AttackAnim->Play();
        }
    }

    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_AttackTimer += dt;

    if (m_State == State::WALK) {
        // 移動方向對調：
        // 貓咪 (CAT) 現在往左走 (x 減少)
        // 敵人 (ENEMY) 現在往右走 (x 增加)
        if (m_Team == Team::CAT) {
            m_Transform.translation.x -= m_Stats.speed * dt;
        } else {
            m_Transform.translation.x += m_Stats.speed * dt;
        }
    }
}

void Unit::TakeDamage(float damage) {
    m_CurrentHP -= damage;
    if (m_CurrentHP <= 0) {
        m_State = State::DEAD;
    }
}

bool Unit::CanAttack() {
    return m_AttackTimer >= m_Stats.attackInterval;
}

void Unit::ResetAttackTimer() {
    m_AttackTimer = 0.0f;
}
