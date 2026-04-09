#ifndef UNIT_MANAGER_HPP
#define UNIT_MANAGER_HPP

#include "Tower.hpp"
#include "Unit.hpp"
#include "Util/GameObject.hpp"

#include <memory>
#include <vector>

class UnitManager {
public:
    explicit UnitManager(std::shared_ptr<Util::GameObject> root);

    void SetBases(std::shared_ptr<Tower> catBase, std::shared_ptr<Tower> enemyBase);
    void AddUnit(std::shared_ptr<Unit> unit);
    void Update();
    void ClearUnits();

    bool IsGameOver() const;
    std::string GetWinner() const;

private:
    void HandleCollisionAndCombat();
    void CleanupDeadUnits();

    std::shared_ptr<Util::GameObject> m_Root;
    std::vector<std::shared_ptr<Unit>> m_Units;

    std::shared_ptr<Tower> m_CatBase;
    std::shared_ptr<Tower> m_EnemyBase;
};

#endif
