#ifndef UNIT_DATA_HPP
#define UNIT_DATA_HPP

#include "Unit.hpp"
#include <string>
#include <vector>

struct UnitData {
    std::string name;
    Unit::Stats stats;
    std::string resourcePath; // 例如 "/Units/Cats/BasicCat"
    int walkFrames;
    int attackFrames;
    float cost;         // 出兵成本
    float cooldown;     // 冷卻時間 (秒)
    float yOffset;      // 新增：垂直偏移量（用於漂浮單位）
    bool isAreaAttack;  // 新增：是否為範圍攻擊
    int knockbackCount; // 新增：總擊退次數
    std::string iconPath; // 按鈕圖示路徑
};

#endif
