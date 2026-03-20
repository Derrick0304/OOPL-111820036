#include "Unit.hpp"
#include "Util/Time.hpp"
#include <cmath>

Unit::Unit(Team team, const Stats &stats, 
           const std::vector<std::string> &walkPaths, 
           const std::vector<std::string> &attackPaths,
           float yOffset)
    : m_Team(team), m_Stats(stats), m_YOffset(yOffset), m_CurrentHP(stats.maxHp) {
    
    m_WalkAnim = std::make_shared<Util::Animation>(walkPaths, true, 200, true);
    m_AttackAnim = std::make_shared<Util::Animation>(attackPaths, false, 150, false);
    
    if (!walkPaths.empty()) {
        m_KnockbackImage = std::make_shared<Util::Image>(walkPaths[0]);
    }

    if (m_Stats.knockbackCount > 0) {
        m_KnockbackHPStep = m_Stats.maxHp / (m_Stats.knockbackCount + 1);
        m_NextKnockbackHP = m_Stats.maxHp - m_KnockbackHPStep;
    } else {
        m_NextKnockbackHP = -1.0f;
    }

    m_State = State::WALK;
    SetDrawable(m_WalkAnim);
    m_WalkAnim->Play();

    m_Pivot = {0, 0}; 
    m_Transform.scale = {1.0f, 1.0f};
    m_Transform.translation.x = (m_Team == Team::CAT) ? 400.0f : -400.0f;
    m_Transform.translation.y = -150.0f; 
    m_ZIndex = 1.0f;
}

void Unit::TriggerAttackAnimation() {
    if (m_State != State::ATTACK && m_State != State::KNOCKBACK && m_State != State::DEAD) {
        SetState(State::ATTACK);
    }
}

void Unit::SetState(State state) {
    if (m_State == state) return; 
    
    if (m_State == State::DEAD) return; 
    if (m_State == State::KNOCKBACK && state != State::DEAD) return;
    if (m_State == State::ATTACK && state == State::WALK) {
        if (!IsAttackAnimationEnded()) return;
    }

    m_State = state;
    if (m_State == State::WALK) {
        SetDrawable(m_WalkAnim);
        m_WalkAnim->Play();
        m_Transform.rotation = 0.0f;
    } else if (m_State == State::ATTACK) {
        SetDrawable(m_AttackAnim);
        m_AttackAnim->SetCurrentFrame(0);
        m_AttackAnim->Play();
        m_Transform.rotation = 0.0f;
    } else if (m_State == State::KNOCKBACK) {
        SetDrawable(m_KnockbackImage);
        m_KnockbackTimer = 0.5f;
    } else if (m_State == State::DEAD) {
        SetDrawable(m_KnockbackImage);
        m_DeadTimer = 0.8f; // 死亡演出長度
        m_ZIndex = 50.0f;
    }
}

bool Unit::IsAttackAnimationEnded() const {
    return m_AttackAnim->GetState() == Util::Animation::State::ENDED;
}

void Unit::Update() {
    if (m_Drawable) {
        glm::vec2 currentSize = m_Drawable->GetSize();
        float pivotX = (m_Team == Team::CAT) ? currentSize.x / 2.0f : -currentSize.x / 2.0f;
        m_Pivot = {pivotX, -currentSize.y / 2.0f - m_YOffset};
    }

    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // --- 死亡演出的更新 (融合擊退效果) ---
    if (m_State == State::DEAD) {
        m_DeadTimer -= dt;
        
        // 1. 向後飛更遠：速度比擊退快一點 (250.0f)
        float deadFlySpeed = 250.0f;
        if (m_Team == Team::CAT) m_Transform.translation.x += deadFlySpeed * dt;
        else m_Transform.translation.x -= deadFlySpeed * dt;

        // 2. 傾斜與跳動：沿用受傷動畫
        m_Transform.rotation = (m_Team == Team::CAT) ? -0.4f : 0.4f; // 傾斜角加大
        m_Transform.translation.y = -150.0f + std::abs(std::sin(m_DeadTimer * 15.0f)) * 30.0f; // 向上跳動感

        // 3. 逐漸縮小消散
        float scale = std::max(0.0f, m_DeadTimer / 0.8f);
        m_Transform.scale = {scale, scale};
        return;
    }

    // --- 擊退邏輯與動畫 ---
    if (m_State == State::KNOCKBACK) {
        m_KnockbackTimer -= dt;
        float kbSpeed = 150.0f; 
        if (m_Team == Team::CAT) m_Transform.translation.x += kbSpeed * dt;
        else m_Transform.translation.x -= kbSpeed * dt;

        m_Transform.rotation = (m_Team == Team::CAT) ? -0.3f : 0.3f;
        m_Transform.translation.y = -150.0f + std::sin(m_KnockbackTimer * 20.0f) * 10.0f;

        if (m_KnockbackTimer <= 0) {
            m_Transform.translation.y = -150.0f;
            m_Transform.rotation = 0.0f;
            m_State = State::WALK;
            SetDrawable(m_WalkAnim);
        }
        return;
    }

    if (m_State == State::ATTACK && IsAttackAnimationEnded()) {
        m_AttackAnim->SetCurrentFrame(0);
        m_AttackAnim->Play();
    }

    m_AttackTimer += dt;

    if (m_State == State::WALK) {
        if (m_Team == Team::CAT) m_Transform.translation.x -= m_Stats.speed * dt;
        else m_Transform.translation.x += m_Stats.speed * dt;
    }
}

void Unit::TakeDamage(float damage) {
    if (m_State == State::DEAD) return;

    m_CurrentHP -= damage;
    
    // 先檢查是否死亡，如果沒死再檢查擊退
    if (m_CurrentHP <= 0) {
        SetState(State::DEAD);
        return;
    }

    if (m_CurrentHP <= m_NextKnockbackHP) {
        SetState(State::KNOCKBACK);
        m_NextKnockbackHP -= m_KnockbackHPStep;
    }
}

bool Unit::CanAttack() {
    return m_AttackTimer >= m_Stats.attackInterval;
}

void Unit::ResetAttackTimer() {
    m_AttackTimer = 0.0f;
}
