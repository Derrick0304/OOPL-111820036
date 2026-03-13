#include "App.hpp"
#include "UnitFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/TransformUtils.hpp"

void App::Start() {
    LOG_TRACE("Game Started");
    
    UnitFactory::Init();
    
    m_Root = std::make_shared<Util::GameObject>();
    m_Renderer.AddChild(m_Root);
    
    m_UnitManager = std::make_unique<UnitManager>(m_Root);
    m_UIManager = std::make_unique<UIManager>(m_Root, m_UnitManager.get(), [this](float amount) {
        this->m_Money -= amount;
    });
    // 1. 初始化背景 (縮放至 1.7 並上移)
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/background.png");
    auto bgObj = std::make_shared<Util::GameObject>(m_BackgroundImage, -10.0f);
    bgObj->m_Transform.scale = {1.7f, 1.7f};
    bgObj->m_Transform.translation.y = 50.0f; // 調高背景
    m_Root->AddChild(bgObj);

    // 2. 初始化基地 (調高座標)
    auto catBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/Towers/CatBase/base.png");
    catBase->m_Transform.scale = {1.2f, 1.2f};
    catBase->m_Transform.translation = {500.0f, -50.0f}; // 從 -100.0f 調高

    auto enemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, 1000.0f, RESOURCE_DIR"/Towers/EnemyBase/base.png");
    enemyBase->m_Transform.scale = {0.9f, 0.9f};
    enemyBase->m_Transform.translation = {-500.0f, -50.0f}; // 從 -100.0f 調高


    m_UnitManager->SetBases(catBase, enemyBase);
    
    m_State = State::UPDATE;
}

void App::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    
    // 1. 金錢隨時間增加
    m_Money += 50.0f * dt; // 每秒 50 元

    // 2. 出兵按鈕與出兵扣錢 (透過回調處理)
    // 這裡我們稍微修改 UIManager 以支援扣錢回調
    // (為了示範，我會在 UIManager 中直接使用 App 的 m_Money)
    
    // 3. 自動產生敵人 (簡易 AI 測試)
    static float enemyTimer = 0.0f;
    enemyTimer += dt;
    if (enemyTimer > 5.0f) { // 每 5 秒出一隻狗狗
        m_UnitManager->AddUnit(UnitFactory::Create("Doge", Unit::Team::ENEMY));
        enemyTimer = 0.0f;
    }

    m_UnitManager->Update();
    m_UIManager->Update(m_Money); // 傳遞金錢給 UI 更新按鈕狀態
    m_Renderer.Update();

    if (m_UnitManager->IsGameOver()) {
        LOG_CRITICAL("GAME OVER: {}", m_UnitManager->GetWinner());
        m_State = State::END;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_State = State::END;
    }
}

void App::End() {
    LOG_TRACE("Game Ended");
}
