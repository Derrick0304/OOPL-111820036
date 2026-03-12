#include "UnitManager.hpp"
#include "Util/Logger.hpp"
#include <algorithm>
#include <cmath>

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
        
        // 1. 檢查與敵方小兵的碰撞
        for (auto &unitB : m_Units) {
            if (unitA->GetTeam() == unitB->GetTeam()) continue;
            float dist = std::abs(unitA->m_Transform.translation.x - unitB->m_Transform.translation.x);
            if (dist < unitA->GetAttackRange()) {
                blocked = true;
                if (unitA->CanAttack()) {
                    unitB->TakeDamage(unitA->GetAttackDamage());
                    unitA->ResetAttackTimer();
                }
                break;
            }
        }
        
        // 2. 如果沒被小兵擋住，檢查是否在敵方基地射程內
        if (!blocked) {
            auto enemyBase = (unitA->GetTeam() == Unit::Team::CAT) ? m_EnemyBase : m_CatBase;
            if (enemyBase) {
                float distToBase = std::abs(unitA->m_Transform.translation.x - enemyBase->m_Transform.translation.x);
                if (distToBase < unitA->GetAttackRange() + 50.0f) { // 基地體積較大，加成射程
                    blocked = true;
                    if (unitA->CanAttack()) {
                        enemyBase->TakeDamage(unitA->GetAttackDamage());
                        unitA->ResetAttackTimer();
                    }
                }
            }
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
    
    // 基地死亡不移除，改由 Game Over 邏輯處理
}

bool UnitManager::IsGameOver() const {
    return (m_CatBase && m_CatBase->IsDead()) || (m_EnemyBase && m_EnemyBase->IsDead());
}

std::string UnitManager::GetWinner() const {
    if (m_EnemyBase && m_EnemyBase->IsDead()) return "CATS WIN!";
    if (m_CatBase && m_CatBase->IsDead()) return "ENEMIES WIN!";
    return "";
}
