#ifndef UNIT_HPP
#define UNIT_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include <map>
#include <string>
#include <vector>

class Unit : public Util::GameObject {
public:
    enum class Team { CAT, ENEMY };
    enum class State { WALK, ATTACK, DEAD };

    struct Stats {
        float maxHp;
        float speed;
        float attackRange;
        float attackDamage;
        float attackInterval;
    };

    // 建構子改為接收動畫路徑列表與 yOffset
    Unit(Team team, const Stats &stats, 
         const std::vector<std::string> &walkPaths, 
         const std::vector<std::string> &attackPaths,
         float yOffset = 0.0f);

    virtual void Update();
    
    void TakeDamage(float damage);
    bool CanAttack();
    void ResetAttackTimer();
    
    Team GetTeam() const { return m_Team; }
    State GetState() const { return m_State; }
    void SetState(State state); // 修改狀態時自動切換動畫
    bool IsAttackAnimationEnded() const; // 檢查攻擊動畫是否播完
    
    float GetAttackDamage() const { return m_Stats.attackDamage; }
    float GetAttackRange() const { return m_Stats.attackRange; }
    bool IsDead() const { return m_CurrentHP <= 0; }
    float GetHP() const { return m_CurrentHP; }

protected:
    Team m_Team;
    State m_State = State::WALK;
    Stats m_Stats;
    float m_YOffset; // 新增偏移量
    
    float m_CurrentHP;
    float m_AttackTimer = 0.0f;

    // 動態存儲動畫物件
    std::shared_ptr<Util::Animation> m_WalkAnim;
    std::shared_ptr<Util::Animation> m_AttackAnim;
};

#endif
