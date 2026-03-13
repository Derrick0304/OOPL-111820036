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
};

#endif
