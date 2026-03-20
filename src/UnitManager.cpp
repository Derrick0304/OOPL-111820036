#include "UnitManager.hpp"
#include "Util/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

UnitManager::UnitManager(std::shared_ptr<Util::GameObject> root) : m_Root(root) {}

void UnitManager::SetBases(std::shared_ptr<Tower> catBase, std::shared_ptr<Tower> enemyBase) {
    m_CatBase = catBase;
    m_EnemyBase = enemyBase;
    m_Root->AddChild(m_CatBase);
    m_Root->AddChild(m_EnemyBase);
}

void UnitManager::AddUnit(std::shared_ptr<Unit> unit) {
    m_Units.push_back(unit);
    m_Root->AddChild(unit);
}

void UnitManager::Update() {
    HandleCollisionAndCombat();
    
    for (auto &unit : m_Units) unit->Update();
    if (m_CatBase) m_CatBase->Update();
    if (m_EnemyBase) m_EnemyBase->Update();
    
    CleanupDeadUnits();
}

void UnitManager::HandleCollisionAndCombat() {
    for (auto &unitA : m_Units) {
        bool blocked = false;
        float widthA = unitA->GetScaledSize().x;
        float frontA = (unitA->GetTeam() == Unit::Team::CAT) ? 
                        unitA->m_Transform.translation.x - widthA : 
                        unitA->m_Transform.translation.x + widthA;

        std::vector<std::shared_ptr<Unit>> targetsInRange;

        // 1. 搜集射程內的所有敵人
        for (auto &unitB : m_Units) {
            if (unitA->GetTeam() == unitB->GetTeam()) continue;
            
            float widthB = unitB->GetScaledSize().x;
            float frontB = (unitB->GetTeam() == Unit::Team::CAT) ? 
                            unitB->m_Transform.translation.x - widthB : 
                            unitB->m_Transform.translation.x + widthB;

            float dist = std::abs(frontA - frontB);

            if (dist < unitA->GetAttackRange()) {
                blocked = true;
                targetsInRange.push_back(unitB);
            }
        }
        
        // 2. 檢查敵方基地
        std::shared_ptr<Tower> targetBase = nullptr;
        if (!blocked) {
            auto enemyBase = (unitA->GetTeam() == Unit::Team::CAT) ? m_EnemyBase : m_CatBase;
            if (enemyBase) {
                float distToBase = std::abs(frontA - enemyBase->m_Transform.translation.x);
                if (distToBase < unitA->GetAttackRange() + 20.0f) { 
                    blocked = true;
                    targetBase = enemyBase;
                }
            }
        }
        
        // 3. 執行傷害邏輯
        if (blocked && unitA->CanAttack()) {
            // 對小兵造成傷害
            if (!targetsInRange.empty()) {
                if (unitA->IsAreaAttack()) {
                    // 範圍攻擊：傷害所有目標
                    for (auto &target : targetsInRange) {
                        target->TakeDamage(unitA->GetAttackDamage());
                    }
                } else {
                    // 單體攻擊：僅傷害最前面的目標 (這裡假設搜集順序即為位置順序，或可再精確排序)
                    targetsInRange[0]->TakeDamage(unitA->GetAttackDamage());
                }
            }

            // 對基地造成傷害
            if (targetBase) {
                targetBase->TakeDamage(unitA->GetAttackDamage());
            }

            unitA->TriggerAttackAnimation();
            unitA->ResetAttackTimer();
        }

        unitA->SetState(blocked ? Unit::State::ATTACK : Unit::State::WALK);
    }
}

void UnitManager::CleanupDeadUnits() {
    auto it = std::remove_if(m_Units.begin(), m_Units.end(), [this](const std::shared_ptr<Unit> &u) {
        if (u->IsDead()) {
            m_Root->RemoveChild(u);
            return true;
        }
        return false;
    });
    m_Units.erase(it, m_Units.end());
}

bool UnitManager::IsGameOver() const {
    return (m_CatBase && m_CatBase->IsDead()) || (m_EnemyBase && m_EnemyBase->IsDead());
}

std::string UnitManager::GetWinner() const {
    if (m_EnemyBase && m_EnemyBase->IsDead()) return "CATS WIN!";
    if (m_CatBase && m_CatBase->IsDead()) return "ENEMIES WIN!";
    return "";
}
