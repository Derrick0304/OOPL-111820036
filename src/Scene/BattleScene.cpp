#include "Scene/BattleScene.hpp"

#include "App.hpp"
#include "Core/Context.hpp"
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
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App));
        return;
    }
    if (Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    // --- Camera Handling ---
    glm::vec2 cursor = Util::Input::GetCursorPosition();

    // Only start dragging if not interacting with UI (simplified check: if y < 200, assume gameplay area)
    // Actually, let's just allow drag anywhere for now, but ensure the state transitions are correct.
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_IsDragging = true;
        m_LastMouseX = cursor.x;
    }

    if (m_IsDragging) {
        if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
            float dx = cursor.x - m_LastMouseX;
            m_TargetCameraX -= dx;
            m_LastMouseX = cursor.x;
        } 
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
            m_IsDragging = false;
        }
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        m_TargetCameraX -= 1500.0f * dt;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        m_TargetCameraX += 1500.0f * dt;
    }

    float halfWinWidth = Core::Context::GetInstance()->GetWindowWidth() / 2.0f;
    float maxCamX = (m_Stage.stageLength / 2.0f) - halfWinWidth;
    if (maxCamX < 0) maxCamX = 0;
    m_TargetCameraX = std::max(-maxCamX, std::min(m_TargetCameraX, maxCamX));

    float oldCameraX = m_CameraX;
    m_CameraX += (m_TargetCameraX - m_CameraX) * 15.0f * dt;
    float dx = m_CameraX - oldCameraX;

    if (dx != 0.0f) {
        for (auto& bg : m_Backgrounds) {
            bg->m_Transform.translation.x -= dx;
        }
        if (m_UnitManager) {
            m_UnitManager->ShiftAll(-dx);
        }
    }

    if (m_BattleEnded) {
        m_RetryButton->Update();
        m_StageSelectButton->Update();
        m_MainMenuButton->Update();
        return;
    }

    m_Money += m_MoneyPerSecond * dt;
    if (m_Money > m_MaxMoney) {
        m_Money = m_MaxMoney;
    }

    if (m_CannonCooldown < CANNON_MAX_COOLDOWN) {
        m_CannonCooldown += dt;
    }

    if (m_CatCannonEffect) {
        m_CatCannonEffect->Update(dt);
    }

    // --- 貓咪砲傷害波邏輯 ---
    if (m_CannonEffectTimer >= 0.0f) {
        m_CannonEffectTimer += dt;
        
        if (m_CannonEffectTimer >= 0.4f && m_CannonEffectTimer <= 0.9f) {
            float progress = (m_CannonEffectTimer - 0.4f) / 0.5f;
            float startX = m_Stage.stageLength / 2.0f;
            float endX = -m_Stage.stageLength / 2.0f;
            float currentX = startX + (endX - startX) * progress;
            
            if (m_UnitManager) {
                m_UnitManager->ApplyCannonDamageInArea(currentX - 50.0f, currentX + 50.0f, 200.0f, m_CannonHitList);
            }
        }
        
        if (m_CannonEffectTimer > 1.5f) {
            m_CannonEffectTimer = -1.0f;
            m_CannonHitList.clear();
        }
    }

    if (m_WaveSpawner) {
        m_WaveSpawner->Update(dt);
    }
    if (m_UnitManager) {
        m_UnitManager->Update();
    }
    if (m_UIManager) {
        float nextCost = (m_WorkerLevel < 8) ? m_WorkerUpgradeCosts[m_WorkerLevel - 1] : 0.0f;
        float cannonProgress = std::min(m_CannonCooldown / CANNON_MAX_COOLDOWN, 1.0f);
        m_UIManager->Update(m_Money, m_WorkerLevel, nextCost, cannonProgress);
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
    m_WorldRoot.reset();
    m_Backgrounds.clear();
}

void BattleScene::SetupBattlefield() {
    m_WorldRoot = std::make_shared<Util::GameObject>();
    m_Root->AddChild(m_WorldRoot);

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR + m_Stage.background);
    float bgWidth = m_BackgroundImage->GetSize().x * 1.7f;
    int numTiles = static_cast<int>(std::ceil(m_Stage.stageLength / bgWidth)) + 2;

    for (int i = -1; i < numTiles; i++) {
        auto bgObj = std::make_shared<Util::GameObject>(m_BackgroundImage, -10.0f);
        bgObj->m_Transform.scale = {1.7f, 1.7f};
        float startX = -m_Stage.stageLength / 2.0f;
        bgObj->m_Transform.translation.x = startX + (bgWidth / 2.0f) + i * bgWidth;
        bgObj->m_Transform.translation.y = 50.0f;
        m_WorldRoot->AddChild(bgObj);
        m_Backgrounds.push_back(bgObj);
    }

    m_StageTitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 24, m_Stage.displayName);
    m_StageTitleObject = std::make_shared<Util::GameObject>(m_StageTitleText, 15.0f);
    m_StageTitleObject->m_Transform.translation = {-500.0f, 320.0f};
    m_Root->AddChild(m_StageTitleObject);

    m_UnitManager = std::make_unique<UnitManager>(m_WorldRoot);
    
    m_MaxMoney = m_MaxMoneyLevels[0];
    m_MoneyPerSecond = m_MoneyRateLevels[0];
    m_WorkerLevel = 1;

    m_CatCannonEffect = std::make_unique<CatCannonEffect>(m_WorldRoot);

    m_UIManager = std::make_unique<UIManager>(m_Root, m_UnitManager.get(), 
        m_App.GetEquippedCats(),
        [this](float amount) {
            if (m_Money < amount) return false;
            m_Money -= amount;
            return true;
        },
        [this]() {
            if (m_WorkerLevel < 8) {
                float cost = m_WorkerUpgradeCosts[m_WorkerLevel - 1];
                if (m_Money >= cost) {
                    m_Money -= cost;
                    m_WorkerLevel++;
                    m_MaxMoney = m_MaxMoneyLevels[m_WorkerLevel - 1];
                    m_MoneyPerSecond = m_MoneyRateLevels[m_WorkerLevel - 1];
                }
            }
        },
        [this]() {
            if (m_CannonCooldown >= CANNON_MAX_COOLDOWN) {
                m_CannonCooldown = 0.0f;
                // Calculate correct absolute edges for cannon effect based on current base positions
                float catBaseX = m_UnitManager->GetCatBaseX();
                float enemyBaseX = m_UnitManager->GetEnemyBaseX();
                m_CatCannonEffect->Trigger(catBaseX, enemyBaseX, -150.0f);
                m_CannonEffectTimer = 0.0f;
                m_CannonHitList.clear();
            }
        }
    );

    auto catBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/Towers/CatBase/base.png");
    catBase->m_Transform.scale = {1.2f, 1.2f};
    catBase->m_Transform.translation = {m_Stage.stageLength / 2.0f, -150.0f};

    auto enemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, m_Stage.enemyBaseHp, RESOURCE_DIR"/Towers/EnemyBase/base.png");
    enemyBase->m_Transform.scale = {0.9f, 0.9f};
    enemyBase->m_Transform.translation = {-m_Stage.stageLength / 2.0f, -150.0f};

    m_UnitManager->SetBases(catBase, enemyBase);
    m_WaveSpawner = std::make_unique<WaveSpawner>(m_Stage, m_UnitManager.get());

    // Initialize Camera Position (Look at Cat Base)
    float halfWinWidth = Core::Context::GetInstance()->GetWindowWidth() / 2.0f;
    float maxCamX = (m_Stage.stageLength / 2.0f) - halfWinWidth;
    if (maxCamX < 0) maxCamX = 0;
    m_CameraX = maxCamX;
    m_TargetCameraX = maxCamX;
    
    // Apply initial shift manually since we bypassed m_WorldRoot transform
    for (auto& bg : m_Backgrounds) {
        bg->m_Transform.translation.x -= m_CameraX;
    }
    m_UnitManager->ShiftAll(-m_CameraX);
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
