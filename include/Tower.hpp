#ifndef TOWER_HPP
#define TOWER_HPP

#include "Unit.hpp"

class Tower : public Unit {
public:
    Tower(Team team, float maxHp, const std::string &imagePath)
        : Unit(team, Stats{maxHp, 0.0f, 0.0f, 0.0f, 100.0f}, {imagePath}, {imagePath}) {
        
        m_Transform.translation.x = (m_Team == Team::CAT) ? -600.0f : 600.0f;
        m_Transform.translation.y = -50.0f;
        m_Transform.scale = {1.33f, 1.33f}; 
        
        // 基地圖層設為比背景高，但比小兵低
        m_ZIndex = -1.0f;
    }

    void Update() override {
        Unit::Update();
    }
};

#endif
