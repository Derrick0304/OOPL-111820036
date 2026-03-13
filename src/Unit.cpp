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

void Unit::SetState(State state) {
    if (m_State == state) return; 
    
    // 如果現在在攻擊，且動畫還沒結束，則不允許切換到 WALK
    if (m_State == State::ATTACK && state == State::WALK) {
        if (!IsAttackAnimationEnded()) return;
    }

    m_State = state;
    if (m_State == State::WALK) {
        SetDrawable(m_WalkAnim);
        m_WalkAnim->Play();
    } else if (m_State == State::ATTACK) {
        SetDrawable(m_AttackAnim);
        m_AttackAnim->SetCurrentFrame(0); // 每次攻擊都從第 0 幀開始
        m_AttackAnim->Play();
    }
}

bool Unit::IsAttackAnimationEnded() const {
    // Util::Animation::GetState() 會在一次性動畫播完後返回 ENDED
    return m_AttackAnim->GetState() == Util::Animation::State::ENDED;
}

void Unit::Update() {
    // --- 動態更新 Pivot 以確保底部對齊 (並加入 yOffset 偏移) ---
    if (m_Drawable) {
        glm::vec2 currentSize = m_Drawable->GetSize();
        // 將 Pivot 向下推，圖片就會向上浮起
        m_Pivot = {0, -currentSize.y / 2.0f - m_YOffset};
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
