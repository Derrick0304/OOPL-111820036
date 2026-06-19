#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Scene/IScene.hpp"
#include "Util/Renderer.hpp"

#include <memory>
#include <vector>
#include <string>

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_State; }

    void Start();
    void Update();
    void End();
    void ChangeScene(std::unique_ptr<IScene> nextScene);
    void RequestExit() { m_State = State::END; }

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

private:
    void ApplyPendingSceneChange();

    State m_State = State::START;
    Util::Renderer m_Renderer;
    std::unique_ptr<IScene> m_CurrentScene;
    std::unique_ptr<IScene> m_PendingScene;
    bool m_IsUpdatingScene = false;

    int m_TotalXP = 85735;
    int m_CatFood = 31;
    int m_CurrentEnergy = 294;
    int m_MaxEnergy = 999;
    float m_EnergyRecoveryTimer = 0.0f;

    std::vector<std::string> m_EquippedCats = { "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat", "", "", "", "", "" };
};

#endif
