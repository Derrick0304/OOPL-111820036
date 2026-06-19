#include "App.hpp"
#include "Util/BGM.hpp"

#include "Scene/MainMenuScene.hpp"
#include "Scene/StartScene.hpp"
#include "Stage/StageLoader.hpp"
#include "UnitFactory.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

void App::Start() {
    LOG_TRACE("Game Started");

    UnitFactory::Init();
    StageLoader::Load();

    std::vector<std::string> cats = { "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat", "BirdCat", "FishCat", "LizardCat", "TitanCat", "KillerCat" };
    for (auto& c : cats) {
        m_CatLevels[c] = 1;
    }

    auto stages = StageLoader::GetAllStages();
    for (auto& s : stages) {
        m_StageClearCounts[s.id] = 0;
    }

    ChangeScene(std::make_unique<StartScene>(*this));

    m_State = State::UPDATE;
}

void App::Update() {
    if (m_CurrentScene) {
        m_IsUpdatingScene = true;
        m_CurrentScene->Update();
        m_IsUpdatingScene = false;
    }

    // 體力自動回復邏輯 (每 10 秒回復 1 點體力)
    if (m_CurrentEnergy < m_MaxEnergy) {
        m_EnergyRecoveryTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
        if (m_EnergyRecoveryTimer >= 10.0f) {
            m_CurrentEnergy = std::min(m_MaxEnergy, m_CurrentEnergy + 1);
            m_EnergyRecoveryTimer = 0.0f;
        }
    } else {
        m_EnergyRecoveryTimer = 0.0f;
    }

    ApplyPendingSceneChange();
    m_Renderer.Update();
}

void App::End() {
    if (m_CurrentScene) {
        auto root = m_CurrentScene->GetRoot();
        m_CurrentScene->Exit();
        if (root) {
            m_Renderer.RemoveChild(root);
        }
        m_CurrentScene.reset();
    }

    LOG_TRACE("Game Ended");
}

void App::ChangeScene(std::unique_ptr<IScene> nextScene) {
    if (m_IsUpdatingScene) {
        m_PendingScene = std::move(nextScene);
        return;
    }

    if (m_CurrentScene) {
        auto currentRoot = m_CurrentScene->GetRoot();
        m_CurrentScene->Exit();
        if (currentRoot) {
            m_Renderer.RemoveChild(currentRoot);
        }
    }

    m_CurrentScene = std::move(nextScene);
    if (!m_CurrentScene) {
        return;
    }

    auto nextRoot = m_CurrentScene->GetRoot();
    if (nextRoot) {
        m_Renderer.AddChild(nextRoot);
    }
    m_CurrentScene->Enter();
}

void App::ApplyPendingSceneChange() {
    if (!m_PendingScene) {
        return;
    }

    auto nextScene = std::move(m_PendingScene);
    ChangeScene(std::move(nextScene));
}

void App::PlayBGM(const std::string& path) {
    if (m_CurrentBGMPath == path) {
        return;
    }
    m_BGM = std::make_shared<Util::BGM>(path);
    m_BGM->SetVolume(24); 
    m_BGM->Play();
    m_CurrentBGMPath = path;
}
