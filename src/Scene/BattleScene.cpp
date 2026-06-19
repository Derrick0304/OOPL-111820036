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
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

BattleScene::BattleScene(App& app, StageData stage)
    : m_App(app), m_Stage(std::move(stage)), m_Root(std::make_shared<Util::GameObject>()) {}

void BattleScene::Enter() {
    SetupBattlefield();
    SetupResultOverlay();
}

void BattleScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    if (m_IsPaused) {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            glm::vec2 cursor = Util::Input::GetCursorPosition();

            // 1. 檢測右上角黃色叉叉按鈕
            if (glm::distance(cursor, glm::vec2(205.0f, 175.0f)) < 40.0f) {
                m_IsPaused = false;
                m_SettingsObject->SetVisible(false);
                LOG_INFO("Unpaused game via X button");
            }

            // 2. 檢測中下方 Escape 按鈕
            if (cursor.x >= -155.0f && cursor.x <= 145.0f &&
                cursor.y >= -185.0f && cursor.y <= -115.0f) {
                LOG_INFO("Exited battle from settings via Escape button");
                m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, m_Stage.chapterId));
                return;
            }
        }

        if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
            m_IsPaused = false;
            m_SettingsObject->SetVisible(false);
            LOG_INFO("Unpaused game via ESC key");
        }

        return; // 暫停時跳過所有更新
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_IsPaused = true;
        m_SettingsObject->SetVisible(true);
        LOG_INFO("Game paused via ESC key");
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

    if (m_PauseButton) {
        m_PauseButton->Update();
    }

    // 動態更新雙方基地的血量文字座標與內容 (往中間靠攏 100 像素防超出畫面)
    if (m_CatBase && m_CatBaseHPObject) {
        m_CatBaseHPObject->m_Transform.translation = {m_CatBase->m_Transform.translation.x - 100.0f, m_CatBase->m_Transform.translation.y + 380.0f};
        int currentHp = std::max(0, static_cast<int>(m_CatBase->GetHP()));
        m_CatBaseHPText->SetText(std::to_string(currentHp) + " / " + std::to_string(static_cast<int>(m_CatBase->GetMaxHP())));
    }
    if (m_EnemyBase && m_EnemyBaseHPObject) {
        m_EnemyBaseHPObject->m_Transform.translation = {m_EnemyBase->m_Transform.translation.x + 100.0f, m_EnemyBase->m_Transform.translation.y + 380.0f};
        int currentHp = std::max(0, static_cast<int>(m_EnemyBase->GetHP()));
        m_EnemyBaseHPText->SetText(std::to_string(currentHp) + " / " + std::to_string(static_cast<int>(m_EnemyBase->GetMaxHP())));
    }
}

void BattleScene::Exit() {
    if (m_UnitManager) {
        m_UnitManager->ClearUnits();
    }
    m_Root = std::make_shared<Util::GameObject>();
    m_WorldRoot.reset();
    m_Backgrounds.clear();
    m_PauseButton.reset();
    m_SettingsImage.reset();
    m_SettingsObject.reset();
    m_CatBase.reset();
    m_EnemyBase.reset();
    m_CatBaseHPText.reset();
    m_CatBaseHPObject.reset();
    m_EnemyBaseHPText.reset();
    m_EnemyBaseHPObject.reset();
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

    float titleX = -450.0f, titleY = 320.0f;
    std::ifstream titleFile(RESOURCE_DIR"/Data/UI_Layout.json");
    if (titleFile.is_open()) {
        try {
            json layout = json::parse(titleFile);
            if (layout.contains("BattleScene") && layout["BattleScene"].contains("StageTitle")) {
                titleX = layout["BattleScene"]["StageTitle"]["x"].get<float>();
                titleY = layout["BattleScene"]["StageTitle"]["y"].get<float>();
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json for StageTitle: {}", e.what());
        }
    }

    m_StageTitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 24, m_Stage.displayName);
    m_StageTitleObject = std::make_shared<Util::GameObject>(m_StageTitleText, 15.0f);
    m_StageTitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_StageTitleObject);

    m_UnitManager = std::make_unique<UnitManager>(m_WorldRoot);
    
    m_MaxMoney = m_MaxMoneyLevels[0];
    m_MoneyPerSecond = m_MoneyRateLevels[0];
    m_WorkerLevel = 1;

    m_CatCannonEffect = std::make_unique<CatCannonEffect>(m_WorldRoot);

    m_UIManager = std::make_unique<UIManager>(m_Root, m_UnitManager.get(), 
        m_App.GetEquippedCats(),
        [this](const std::string& catId) {
            return m_App.GetCatLevel(catId);
        },
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

    m_CatBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/Towers/CatBase/base.png");
    m_CatBase->m_Transform.scale = {1.2f, 1.2f};
    m_CatBase->m_Transform.translation = {m_Stage.stageLength / 2.0f, -150.0f};

    m_EnemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, m_Stage.enemyBaseHp, RESOURCE_DIR"/Towers/EnemyBase/base.png");
    m_EnemyBase->m_Transform.scale = {0.9f, 0.9f};
    m_EnemyBase->m_Transform.translation = {-m_Stage.stageLength / 2.0f, -150.0f};

    m_UnitManager->SetBases(m_CatBase, m_EnemyBase);

    // 建立雙方基地的血量顯示文字 (置於塔的上方 +380.0f 位置，往中間靠攏 100 像素防超出畫面)
    m_CatBaseHPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, "1000 / 1000", Util::Color(0, 0, 0)); // 貓咪基地文字改黑色
    m_CatBaseHPObject = std::make_shared<Util::GameObject>(m_CatBaseHPText, 16.0f);
    m_CatBaseHPObject->m_Transform.translation = {m_CatBase->m_Transform.translation.x - 100.0f, m_CatBase->m_Transform.translation.y + 380.0f};
    m_Root->AddChild(m_CatBaseHPObject);

    m_EnemyBaseHPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, std::to_string(static_cast<int>(m_Stage.enemyBaseHp)) + " / " + std::to_string(static_cast<int>(m_Stage.enemyBaseHp)), Util::Color(255, 0, 0)); // 敵人基地文字改紅色
    m_EnemyBaseHPObject = std::make_shared<Util::GameObject>(m_EnemyBaseHPText, 16.0f);
    m_EnemyBaseHPObject->m_Transform.translation = {m_EnemyBase->m_Transform.translation.x + 100.0f, m_EnemyBase->m_Transform.translation.y + 380.0f};
    m_Root->AddChild(m_EnemyBaseHPObject);

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

    // 建立暫停與設定介面
    float pauseX = 580.0f, pauseY = 288.0f;
    std::ifstream uiFile(RESOURCE_DIR"/Data/UI_Layout.json");
    if (uiFile.is_open()) {
        try {
            json layout = json::parse(uiFile);
            if (layout.contains("BattleScene") && layout["BattleScene"].contains("PauseButton")) {
                pauseX = layout["BattleScene"]["PauseButton"]["x"].get<float>();
                pauseY = layout["BattleScene"]["PauseButton"]["y"].get<float>();
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in BattleScene: {}", e.what());
        }
    }

    m_PauseButton = std::make_shared<ImageTextButton>(" ", [this]() {
        m_IsPaused = true;
        if (m_SettingsObject) {
            m_SettingsObject->SetVisible(true);
        }
        LOG_INFO("Game paused");
    }, "/UI/pause.png");
    m_PauseButton->SetFlashEnabled(false);
    m_PauseButton->m_Transform.translation = {pauseX, pauseY};
    m_PauseButton->SetZIndex(15.0f);
    m_Root->AddChild(m_PauseButton);
    for (auto& part : m_PauseButton->GetParts()) {
        m_Root->AddChild(part);
    }

    m_SettingsImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Settingsswitch.png");
    m_SettingsObject = std::make_shared<Util::GameObject>(m_SettingsImage, 30.0f);
    m_SettingsObject->m_Transform.translation = {0.0f, 0.0f};
    m_SettingsObject->m_Transform.scale = {1.0f, 1.0f};
    m_SettingsObject->SetVisible(false);
    m_Root->AddChild(m_SettingsObject);

    m_IsPaused = false;
}

void BattleScene::SetupResultOverlay() {
    m_ResultObject = std::make_shared<Util::GameObject>(nullptr, 20.0f);
    m_ResultObject->m_Transform.translation = {0.0f, 120.0f};
    m_ResultObject->m_Transform.scale = {1.2f, 1.2f};
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

    std::shared_ptr<Util::Image> resultImg;
    if (resultText == "CATS WIN!") {
        resultImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/victory.png");
        m_App.AddXP(m_Stage.xpReward);
        m_App.IncrementStageClearCount(m_Stage.id);
        LOG_INFO("Added {} XP for winning stage and incremented clear count", m_Stage.xpReward);
    } else {
        resultImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/defeat.png");
    }

    if (m_ResultObject && resultImg) {
        m_ResultObject->SetDrawable(resultImg);
        m_ResultObject->SetVisible(true);
    }

    m_RetryButton->SetVisible(true);
    m_StageSelectButton->SetVisible(true);
    m_MainMenuButton->SetVisible(true);
}
