#include "Scene/StageSelectScene.hpp"

#include "App.hpp"
#include "Scene/BattleScene.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Stage/StageLoader.hpp"
#include "UI/ImageTextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <cmath>

using json = nlohmann::json;

StageSelectScene::StageSelectScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void StageSelectScene::Enter() {
    LOG_INFO("Entering StageSelectScene");

    float titleX = 0.0f, titleY = 250.0f;
    float backX = -500.0f, backY = -300.0f;
    float startBtnX = 500.0f, startBtnY = -300.0f;
    m_CarouselY = 0.0f;
    m_SpacingX = 350.0f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("StageSelectScene")) {
                auto& scene = layout["StageSelectScene"];
                if (scene.contains("TitleText")) {
                    titleX = scene["TitleText"]["x"].get<float>();
                    titleY = scene["TitleText"]["y"].get<float>();
                }
                if (scene.contains("BackButton")) {
                    backX = scene["BackButton"]["x"].get<float>();
                    backY = scene["BackButton"]["y"].get<float>();
                }
                if (scene.contains("StartButton")) {
                    startBtnX = scene["StartButton"]["x"].get<float>();
                    startBtnY = scene["StartButton"]["y"].get<float>();
                }
                if (scene.contains("Carousel")) {
                    m_CarouselY = scene["Carousel"]["y"].get<float>();
                    m_SpacingX = scene["Carousel"]["spacingX"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in StageSelectScene: {}", e.what());
        }
    }

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/StageSelect.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.28f, 1.28f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 30.0f};
    m_Root->AddChild(m_BackgroundObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, "Select Stage", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_TitleObject);

    m_Stages = StageLoader::GetAllStages();
    m_StageItems.clear();

    for (size_t i = 0; i < m_Stages.size(); ++i) {
        // 白色長方形佔位符
        auto baseImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
        auto baseObj = std::make_shared<Util::GameObject>(baseImg, 10.0f);
        baseObj->m_Transform.scale = {2.5f, 1.5f};
        
        auto txt = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 30, m_Stages[i].displayName, Util::Color(255, 255, 255));
        auto txtObj = std::make_shared<Util::GameObject>(txt, 11.0f);
        
        m_Root->AddChild(baseObj);
        m_Root->AddChild(txtObj);
        m_StageItems.push_back({baseObj, txtObj, m_Stages[i]});
    }

    m_BackButton = std::make_shared<ImageTextButton>("Back", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) m_Root->AddChild(part);

    m_StartButton = std::make_shared<ImageTextButton>("Battle!", [this]() {
        if (m_CurrentIndex >= 0 && m_CurrentIndex < m_Stages.size()) {
            m_App.ChangeScene(std::make_unique<BattleScene>(m_App, m_Stages[m_CurrentIndex]));
        }
    }, ImageTextButton::Type::LONG);
    m_StartButton->m_Transform.translation = {startBtnX, startBtnY};
    m_Root->AddChild(m_StartButton);
    for (auto& part : m_StartButton->GetParts()) m_Root->AddChild(part);

    // 初始化捲動位置，對齊到目前索引
    m_CurrentScrollX = -m_CurrentIndex * m_SpacingX;
    m_TargetScrollX = m_CurrentScrollX;
}

void StageSelectScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    HandleInput();
    UpdateCarousel(dt);

    m_BackButton->Update();
    m_StartButton->Update();
}

void StageSelectScene::HandleInput() {
    glm::vec2 cursor = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_IsDragging = true;
        m_LastMouseX = cursor.x;
    }

    if (m_IsDragging) {
        if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
            float dx = cursor.x - m_LastMouseX;
            m_CurrentScrollX += dx;
            m_TargetScrollX = m_CurrentScrollX;
            m_LastMouseX = cursor.x;
        } else {
            m_IsDragging = false;
            m_CurrentIndex = -std::round(m_CurrentScrollX / m_SpacingX);
            m_CurrentIndex = std::max(0, std::min((int)m_Stages.size() - 1, m_CurrentIndex));
            m_TargetScrollX = -m_CurrentIndex * m_SpacingX;
        }
    }
}

void StageSelectScene::UpdateCarousel(float dt) {
    if (!m_IsDragging) {
        m_CurrentScrollX += (m_TargetScrollX - m_CurrentScrollX) * 10.0f * dt;
    }

    for (int i = 0; i < m_StageItems.size(); ++i) {
        float x = m_CurrentScrollX + i * m_SpacingX;
        m_StageItems[i].base->m_Transform.translation = {x, m_CarouselY};
        m_StageItems[i].textObj->m_Transform.translation = {x, m_CarouselY};

        float dist = std::abs(x);
        float scale = 1.0f;
        if (dist < m_SpacingX) {
            scale = 1.0f + 0.3f * (1.0f - dist / m_SpacingX);
        }
        
        m_StageItems[i].base->m_Transform.scale = {2.5f * scale, 1.5f * scale};
        m_StageItems[i].textObj->m_Transform.scale = {scale, scale};
        
        if (!m_IsDragging && dist < m_SpacingX * 0.5f) {
            m_CurrentIndex = i;
        }
    }
}

void StageSelectScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_StageItems.clear();
}
