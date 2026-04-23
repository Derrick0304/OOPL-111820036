#ifndef UNIT_HPP
#define UNIT_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include <map>
#include <string>
#include <vector>

class Unit : public Util::GameObject {
public:
    enum class Team { CAT, ENEMY };
    enum class State { WALK, ATTACK, KNOCKBACK, DEAD };

    struct Stats {
        float maxHp;
        float speed;
        float attackRange;
        float attackDamage;
        float attackInterval;
        bool isAreaAttack;
        int knockbackCount; // 擊退次數
    };

    Unit(Team team, const Stats &stats, 
         const std::vector<std::string> &walkPaths, 
         const std::vector<std::string> &attackPaths,
         float yOffset = 0.0f);

    virtual void Update();
    
    void TakeDamage(float damage);
    void ForceKnockback();
    bool CanAttack();
    void ResetAttackTimer();
    
    Team GetTeam() const { return m_Team; }
    State GetState() const { return m_State; }
    void SetState(State state);
    void TriggerAttackAnimation();
    bool IsAttackAnimationEnded() const;
    
    bool IsAreaAttack() const { return m_Stats.isAreaAttack; }
    float GetAttackDamage() const { return m_Stats.attackDamage; }
    float GetAttackRange() const { return m_Stats.attackRange; }
    bool IsDead() const { return m_CurrentHP <= 0; }
    bool IsDeadAnimationEnded() const { return m_State == State::DEAD && m_DeadTimer <= 0; } // 新增
    bool IsKnockback() const { return m_State == State::KNOCKBACK; }
    float GetHP() const { return m_CurrentHP; }

protected:
    Team m_Team;
    State m_State = State::WALK;
    Stats m_Stats;
    float m_YOffset;
    
    float m_CurrentHP;
    float m_AttackTimer = 0.0f;
    float m_KnockbackTimer = 0.0f;
    float m_DeadTimer = 0.0f;      // 新增：死亡動畫計時器
    float m_NextKnockbackHP;
    float m_KnockbackHPStep;

    std::shared_ptr<Util::Animation> m_WalkAnim;
    std::shared_ptr<Util::Animation> m_AttackAnim;
    std::shared_ptr<Util::Image> m_KnockbackImage;
};

#endif
