#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/TransformUtils.hpp"

void App::Start() {
    LOG_TRACE("Game Started");
    
    m_Root = std::make_shared<Util::GameObject>();
    m_Renderer.AddChild(m_Root);
    
    m_UnitManager = std::make_unique<UnitManager>(m_Root);
    
    // 1. 初始化背景 (縮放至 1.7 確保填滿視窗寬度)
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/background.png");
    auto bgObj = std::make_shared<Util::GameObject>(m_BackgroundImage, -10.0f);
    bgObj->m_Transform.scale = {1.7f, 1.7f};
    m_Root->AddChild(bgObj);

    // 2. 初始化基地 (左右互換)
    auto catBase = std::make_shared<Tower>(Unit::Team::CAT, 1000.0f, RESOURCE_DIR"/cat_base.png");
    catBase->m_Transform.scale = {1.2f, 1.2f};
    catBase->m_Transform.translation = {500.0f, -100.0f}; // 移至右側

    auto enemyBase = std::make_shared<Tower>(Unit::Team::ENEMY, 1000.0f, RESOURCE_DIR"/enemy_base.png");
    enemyBase->m_Transform.scale = {0.9f, 0.9f};
    enemyBase->m_Transform.translation = {-500.0f, -100.0f}; // 移至左側

    m_UnitManager->SetBases(catBase, enemyBase);
    
    m_State = State::UPDATE;
}

void App::Update() {
    float scrollSpeed = 20.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) m_Root->m_Transform.translation.x += scrollSpeed;
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) m_Root->m_Transform.translation.x -= scrollSpeed;

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        std::vector<std::string> catWalk = { RESOURCE_DIR"/cat_walk_1.png", RESOURCE_DIR"/cat_walk_2.png" };
        std::vector<std::string> catAtk  = { RESOURCE_DIR"/cat_atk_1.png", RESOURCE_DIR"/cat_atk_2.png" };
        std::vector<std::string> enemyWalk = { RESOURCE_DIR"/enemy_walk_1.png", RESOURCE_DIR"/enemy_walk_2.png" };
        std::vector<std::string> enemyAtk  = { RESOURCE_DIR"/enemy_atk_1.png", RESOURCE_DIR"/enemy_atk_2.png" };

        // 貓咪與敵人出生在基地正前方
        m_UnitManager->AddUnit(std::make_shared<Unit>(Unit::Team::CAT, 
                                                    Unit::Stats{100, 150, 60, 20, 0.5f}, 
                                                    catWalk, catAtk));
        m_UnitManager->AddUnit(std::make_shared<Unit>(Unit::Team::ENEMY, 
                                                    Unit::Stats{150, 80, 60, 10, 1.0f}, 
                                                    enemyWalk, enemyAtk));
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
