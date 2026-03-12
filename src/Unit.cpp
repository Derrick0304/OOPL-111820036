#include "Unit.hpp"
#include "Util/Time.hpp"

Unit::Unit(Team team, const Stats &stats, 
           const std::vector<std::string> &walkPaths, 
           const std::vector<std::string> &attackPaths)
    : m_Team(team), m_Stats(stats), m_CurrentHP(stats.maxHp) {
    
    m_WalkAnim = std::make_shared<Util::Animation>(walkPaths, true, 200, true);
    m_AttackAnim = std::make_shared<Util::Animation>(attackPaths, false, 150, true);
    
    m_State = State::WALK;
    SetDrawable(m_WalkAnim);
    m_WalkAnim->Play();
    
    // 縮小為原來的三分之一：原本 3.0f -> 現在 1.0f
    m_Transform.scale = {1.0f, 1.0f};
    
    // 出生點對調：貓咪從右邊 (400) 出生，敵人從左邊 (-400) 出生
    m_Transform.translation.x = (m_Team == Team::CAT) ? 400.0f : -400.0f;
    m_Transform.translation.y = -150.0f; 
    
    m_ZIndex = 1.0f;
}

void Unit::SetState(State state) {
    if (m_State == state) return; 
    
    m_State = state;
    if (m_State == State::WALK) {
        SetDrawable(m_WalkAnim);
        m_WalkAnim->Play();
    } else if (m_State == State::ATTACK) {
        SetDrawable(m_AttackAnim);
        m_AttackAnim->Play();
    }
}

void Unit::Update() {
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
