#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Scene/IScene.hpp"
#include "Util/Renderer.hpp"

#include <memory>
#include <vector>
#include <string>
#include <map>

namespace Util {
class BGM;
}

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_State; }

    void Start();
    void Update();
    void End();
    void ChangeScene(std::unique_ptr<IScene> nextScene);
    void RequestExit() { m_State = State::END; }
    void PlayBGM(const std::string& path);

    Util::Renderer& GetRenderer() { return m_Renderer; }

    int GetTotalXP() const { return m_TotalXP; }
    void SetTotalXP(int xp) { m_TotalXP = xp; }
    void AddXP(int xp) { m_TotalXP += xp; }

    int GetCatFood() const { return m_CatFood; }
    void SetCatFood(int catFood) { m_CatFood = catFood; }
    void AddCatFood(int catFood) { m_CatFood += catFood; }

    int GetCurrentEnergy() const { return m_CurrentEnergy; }
    void SetCurrentEnergy(int energy) { m_CurrentEnergy = energy; }
    void AddEnergy(int energy) { m_CurrentEnergy = std::min(m_MaxEnergy, m_CurrentEnergy + energy); }

    int GetMaxEnergy() const { return m_MaxEnergy; }
    void SetMaxEnergy(int maxEnergy) { m_MaxEnergy = maxEnergy; }

    const std::vector<std::string>& GetEquippedCats() const { return m_EquippedCats; }
    void SetEquippedCat(int index, const std::string& catId) {
        if (index >= 0 && index < 10) {
            m_EquippedCats[index] = catId;
        }
    }

    int GetCatLevel(const std::string& catId) const {
        if (catId.empty()) return 1;
        auto it = m_CatLevels.find(catId);
        if (it != m_CatLevels.end()) {
            return it->second;
        }
        return 1;
    }
    void SetCatLevel(const std::string& catId, int level) {
        m_CatLevels[catId] = level;
    }
    void UpgradeCat(const std::string& catId) {
        m_CatLevels[catId] = std::min(10, GetCatLevel(catId) + 1);
    }

    int GetStageClearCount(const std::string& stageId) const {
        if (stageId.empty()) return 0;
        auto it = m_StageClearCounts.find(stageId);
        if (it != m_StageClearCounts.end()) {
            return it->second;
        }
        return 0;
    }
    void SetStageClearCount(const std::string& stageId, int count) {
        m_StageClearCounts[stageId] = count;
    }
    void IncrementStageClearCount(const std::string& stageId) {
        m_StageClearCounts[stageId] = GetStageClearCount(stageId) + 1;
    }

private:
    void ApplyPendingSceneChange();

    State m_State = State::START;
    Util::Renderer m_Renderer;
    std::unique_ptr<IScene> m_CurrentScene;
    std::unique_ptr<IScene> m_PendingScene;
    bool m_IsUpdatingScene = false;

    int m_TotalXP = 50000;
    int m_CatFood = 10000;
    int m_CurrentEnergy = 294;
    int m_MaxEnergy = 999;
    float m_EnergyRecoveryTimer = 0.0f;

    std::vector<std::string> m_EquippedCats = { "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat", "", "", "", "", "" };
    std::map<std::string, int> m_CatLevels;
    std::map<std::string, int> m_StageClearCounts;

    std::shared_ptr<Util::BGM> m_BGM;
    std::string m_CurrentBGMPath;
};

#endif
