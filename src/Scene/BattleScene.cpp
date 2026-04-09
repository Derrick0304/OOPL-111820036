#include "Scene/BattleScene.hpp"

#include "App.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Tower.hpp"
#include "UI/TextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

BattleScene::BattleScene(App& app, StageData stage)
    : m_App(app), m_Stage(std::move(stage)), m_Root(std::make_shared<Util::GameObject>()) {}

void BattleScene::Enter() {
    SetupBattlefield();
    SetupResultOverlay();
}

void BattleScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App));
        return;
    }
    if (Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    if (m_BattleEnded) {
        m_RetryButton->Update();
        m_StageSelectButton->Update();
        m_MainMenuButton->Update();
        return;
    }

    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_Money += m_MoneyPerSecond * dt;

    if (m_WaveSpawner) {
        m_WaveSpawner->Update(dt);
    }
    if (m_UnitManager) {
        m_UnitManager->Update();
    }
    if (m_UIManager) {
        m_UIManager->Update(m_Money);
    }

    if (m_UnitManager && m_UnitManager->IsGameOver()) {
        ShowResult(m_UnitManager->GetWinner());
    }
}

void BattleScene::Exit() {
    if (m_UnitManager) {
        m_UnitManager->ClearUnits();
    }
    m_Root = std::make_shared<Util::GameObject>();
}

void BattleScene::SetupBattlefield() {
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR + m_Stage.background);
    auto backgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -10.0f);
    backgroundObject->m_Transform.scale = {1.7f, 1.7f};
    backgroundObject->m_Transform.translation.y = 50.0f;
    m_Root->AddChild(backgroundObject);

    m_StageTitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 24, m_Stage.displayName);
    m_StageTitleObject = std::make_shared<Util::GameObject>(m_StageTitleText, 15.0f);
    m_StageTitleObject->m_Transform.translation = {-500.0f, 320.0f};
    m_Root->AddChild(m_StageTitleObject);

    m_UnitManager = std::make_unique<UnitManager>(m_Root);
    m_UIManager = std::make_unique<UIManager>(m_Root, m_UnitManager.get(), [this](float amount) {
        if (m_Money < amount) {
            return false;
        }
        m_Money -= amount;
        return true;
    });

    auto catBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/Towers/CatBase/base.png");
    catBase->m_Transform.scale = {1.2f, 1.2f};
    catBase->m_Transform.translation = {500.0f, -150.0f};

    auto enemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, m_Stage.enemyBaseHp, RESOURCE_DIR"/Towers/EnemyBase/base.png");
    enemyBase->m_Transform.scale = {0.9f, 0.9f};
    enemyBase->m_Transform.translation = {-500.0f, -150.0f};

    m_UnitManager->SetBases(catBase, enemyBase);
    m_WaveSpawner = std::make_unique<WaveSpawner>(m_Stage.waves, m_UnitManager.get());
}

void BattleScene::SetupResultOverlay() {
    m_ResultText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, " ");
    m_ResultObject = std::make_shared<Util::GameObject>(m_ResultText, 20.0f);
    m_ResultObject->m_Transform.translation = {0.0f, 120.0f};
    m_ResultObject->SetVisible(false);
    m_Root->AddChild(m_ResultObject);

    m_RetryButton = std::make_shared<TextButton>("Retry", [this]() {
        m_App.ChangeScene(std::make_unique<BattleScene>(m_App, m_Stage));
    });
    m_RetryButton->m_Transform.translation = {0.0f, 20.0f};
    m_RetryButton->SetVisible(false);
    m_Root->AddChild(m_RetryButton);

    m_StageSelectButton = std::make_shared<TextButton>("Back to Stage Select", [this]() {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App));
    }, 28);
    m_StageSelectButton->m_Transform.translation = {0.0f, -50.0f};
    m_StageSelectButton->SetVisible(false);
    m_Root->AddChild(m_StageSelectButton);

    m_MainMenuButton = std::make_shared<TextButton>("Back to Main Menu", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, 28);
    m_MainMenuButton->m_Transform.translation = {0.0f, -110.0f};
    m_MainMenuButton->SetVisible(false);
    m_Root->AddChild(m_MainMenuButton);
}

void BattleScene::ShowResult(const std::string& resultText) {
    if (m_BattleEnded) {
        return;
    }

    LOG_INFO("Battle finished: {}", resultText);
    m_BattleEnded = true;
    m_ResultText->SetText(resultText);
    m_ResultObject->SetVisible(true);
    m_RetryButton->SetVisible(true);
    m_StageSelectButton->SetVisible(true);
    m_MainMenuButton->SetVisible(true);
}
