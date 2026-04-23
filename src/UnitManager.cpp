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
    if (!unit) return;
    
    if (unit->GetTeam() == Unit::Team::CAT) {
        m_Cats.push_back(unit);
    } else {
        m_Enemies.push_back(unit);
    }
    m_Root->AddChild(unit);
}

void UnitManager::Update() {
    HandleCollisionAndCombat();

    // 更新所有單位
    for (auto& unit : m_Cats) unit->Update();
    for (auto& unit : m_Enemies) unit->Update();
    
    if (m_CatBase) m_CatBase->Update();
    if (m_EnemyBase) m_EnemyBase->Update();

    CleanupDeadUnits();
}

void UnitManager::HandleCollisionAndCombat() {
    // 貓咪攻擊敵人
    ProcessTeamBattle(m_Cats, m_Enemies, m_EnemyBase);
    // 敵人攻擊貓咪
    ProcessTeamBattle(m_Enemies, m_Cats, m_CatBase);
}

void UnitManager::ProcessTeamBattle(std::vector<std::shared_ptr<Unit>>& attackers, 
                                   std::vector<std::shared_ptr<Unit>>& defenders, 
                                   std::shared_ptr<Tower> enemyBase) {
    for (auto& unitA : attackers) {
        if (unitA->IsKnockback() || unitA->IsDead()) continue;

        bool blocked = false;
        const float widthA = unitA->GetScaledSize().x;
        
        // 根據 PTSD Draw() 與 Unit.cpp m_Pivot 實作：
        // CAT: PivotX = w/2 -> 渲染於 [x-w, x] -> 前緣在 x - w
        // ENEMY: PivotX = -w/2 -> 渲染於 [x, x+w] -> 前緣在 x + w
        const float frontA = (unitA->GetTeam() == Unit::Team::CAT)
                                 ? unitA->m_Transform.translation.x - widthA
                                 : unitA->m_Transform.translation.x + widthA;

        std::vector<std::shared_ptr<Unit>> targetsInRange;

        // 1. 偵測敌方單位
        for (auto& unitB : defenders) {
            if (unitB->IsDead()) continue;
            
            const float widthB = unitB->GetScaledSize().x;
            const float frontB = (unitB->GetTeam() == Unit::Team::CAT)
                                     ? unitB->m_Transform.translation.x - widthB
                                     : unitB->m_Transform.translation.x + widthB;

            // 計算實質間距 (正值代表尚未接觸)
            // 如果我是 CAT，我在右邊，敵人前緣在我的左邊
            // 距離 = 我的前緣 - 敵人的前緣
            float realDist = (unitA->GetTeam() == Unit::Team::CAT) 
                             ? (frontA - frontB) 
                             : (frontB - frontA);

            if (realDist >= 0 && realDist < unitA->GetAttackRange()) {
                blocked = true;
                targetsInRange.push_back(unitB);
            }
        }

        // 2. 若無單位，則偵測基地
        std::shared_ptr<Tower> targetBase = nullptr;
        if (targetsInRange.empty() && enemyBase && !enemyBase->IsDead()) {
            // 基地寬度同樣受 Pivot 影響
            const float baseWidth = enemyBase->GetScaledSize().x;
            // 敵方基地的「受擊點」
            // 若我是 CAT (向左)，敵方基地是 ENEMY (渲染於 [x, x+w]) -> 受擊點在 x
            // 若我是 ENEMY (向右)，敵方基地是 CAT (渲染於 [x-w, x]) -> 受擊點在 x
            const float enemyBaseHitPoint = enemyBase->m_Transform.translation.x;

            float distToBase = (unitA->GetTeam() == Unit::Team::CAT)
                                ? (frontA - enemyBaseHitPoint)
                                : (enemyBaseHitPoint - frontA);
                                
            if (distToBase >= 0 && distToBase < unitA->GetAttackRange()) {
                blocked = true;
                targetBase = enemyBase;
            }
        }

        // 執行攻擊邏輯
        if (blocked) {
            if (unitA->CanAttack()) {
                if (!targetsInRange.empty()) {
                    if (unitA->IsAreaAttack()) {
                        for (auto& target : targetsInRange) target->TakeDamage(unitA->GetAttackDamage());
                    } else {
                        // 簡單單體攻擊
                        targetsInRange[0]->TakeDamage(unitA->GetAttackDamage());
                    }
                } else if (targetBase) {
                    targetBase->TakeDamage(unitA->GetAttackDamage());
                }

                unitA->TriggerAttackAnimation();
                unitA->ResetAttackTimer();
            }
            unitA->SetState(Unit::State::ATTACK);
        } else {
            unitA->SetState(Unit::State::WALK);
        }
    }
}

void UnitManager::CleanupDeadUnits() {
    auto cleanup = [this](std::vector<std::shared_ptr<Unit>>& units) {
        auto it = std::remove_if(units.begin(), units.end(), [this](const std::shared_ptr<Unit>& u) {
            if (u->IsDeadAnimationEnded()) {
                m_Root->RemoveChild(u);
                return true;
            }
            return false;
        });
        units.erase(it, units.end());
    };

    cleanup(m_Cats);
    cleanup(m_Enemies);
}

bool UnitManager::IsGameOver() const {
    return (m_CatBase && m_CatBase->IsDead()) || (m_EnemyBase && m_EnemyBase->IsDead());
}

std::string UnitManager::GetWinner() const {
    if (m_EnemyBase && m_EnemyBase->IsDead()) return "CATS WIN!";
    if (m_CatBase && m_CatBase->IsDead()) return "ENEMIES WIN!";
    return "";
}

void UnitManager::ApplyCannonDamage(float damage) {
    for (auto& enemy : m_Enemies) {
        if (!enemy->IsDead()) {
            enemy->TakeDamage(damage);
            enemy->ForceKnockback();
        }
    }
}

void UnitManager::ApplyCannonDamageInArea(float minX, float maxX, float damage, std::set<Unit*>& hitList) {
    for (auto& enemy : m_Enemies) {
        if (enemy->IsDead()) continue;
        
        // 檢查該單位是否已經被這發貓咪砲打過了
        if (hitList.find(enemy.get()) != hitList.end()) continue;

        float enemyX = enemy->m_Transform.translation.x;
        // 由於敵人是向右走，我們檢查其中心點是否在範圍內
        // 也可以考慮加上單位寬度判定
        if (enemyX >= minX && enemyX <= maxX) {
            enemy->TakeDamage(damage);
            enemy->ForceKnockback();
            hitList.insert(enemy.get()); // 標記已擊中
        }
    }
}

void UnitManager::ClearUnits() {
    for (auto& unit : m_Cats) m_Root->RemoveChild(unit);
    for (auto& unit : m_Enemies) m_Root->RemoveChild(unit);
    m_Cats.clear();
    m_Enemies.clear();
}
