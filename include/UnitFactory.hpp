#ifndef UNIT_FACTORY_HPP
#define UNIT_FACTORY_HPP

#include "Unit.hpp"
#include "UnitData.hpp"
#include <unordered_map>
#include <memory>
#include <string>

class UnitFactory {
public:
    static void Init();
    static std::shared_ptr<Unit> Create(const std::string& name, Unit::Team team);
    static UnitData Get(const std::string& name) { return s_Registry.at(name); }

private:
    static std::vector<std::string> GeneratePaths(const std::string& folder, const std::string& state, int count);
    static std::unordered_map<std::string, UnitData> s_Registry;
};

#endif
