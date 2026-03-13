#include "App.hpp"
#include "UnitFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/TransformUtils.hpp"

void App::Start() {
    LOG_TRACE("Game Started");
    
    UnitFactory::Init();
    
    m_Root = std::make_shared<Util::GameObject>();
    m_Renderer.AddChild(m_Root);
    
    m_UnitManager = std::make_unique<UnitManager>(m_Root);
    
    // 1. 初始化背景 (縮放至 1.7 確保填滿視窗寬度)
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/background.png");
    auto bgObj = std::make_shared<Util::GameObject>(m_BackgroundImage, -10.0f);
    bgObj->m_Transform.scale = {1.7f, 1.7f};
    m_Root->AddChild(bgObj);

    // 2. 初始化基地 (左右互換，固定在畫面兩側)
    auto catBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/Towers/CatBase/base.png");
    catBase->m_Transform.scale = {1.2f, 1.2f};
    catBase->m_Transform.translation = {500.0f, -100.0f};

    auto enemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, 1000.0f, RESOURCE_DIR"/Towers/EnemyBase/base.png");
    enemyBase->m_Transform.scale = {0.9f, 0.9f};
    enemyBase->m_Transform.translation = {-500.0f, -100.0f};

    m_UnitManager->SetBases(catBase, enemyBase);
    
    m_State = State::UPDATE;
}

void App::Update() {
    // [已移除] 鏡頭捲動邏輯，改為固定視野

    // 出兵測試
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        m_UnitManager->AddUnit(UnitFactory::Create("BasicCat", Unit::Team::CAT));
        m_UnitManager->AddUnit(UnitFactory::Create("Doge", Unit::Team::ENEMY));
    }

    m_UnitManager->Update();
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
