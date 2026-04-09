#ifndef STAGE_LOADER_HPP
#define STAGE_LOADER_HPP

#include "Stage/StageData.hpp"

#include <string>
#include <vector>

class StageLoader {
public:
    static bool Load();
    static const std::vector<StageData>& GetAllStages();
    static const StageData* GetStageById(const std::string& id);

private:
    static std::vector<StageData> s_Stages;
};

#endif
