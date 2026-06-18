#include "Scene/StartScene.hpp"

#include "App.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

StartScene::StartScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void StartScene::Enter() {
    LOG_INFO("Entering StartScene");

    float playX = 0.0f;
    float playY = -200.0f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("StartScene")) {
                auto& scene = layout["StartScene"];
                if (scene.contains("PlayButton")) {
                    playX = scene["PlayButton"]["x"].get<float>();
                    playY = scene["PlayButton"]["y"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in StartScene: {}", e.what());
        }
    }

    // 背景與 ChapterSelectScene 相同
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/ChapterSelectSceneBackgrounds.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.34f, 1.34f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 0.0f};
    m_Root->AddChild(m_BackgroundObject);

    // Play 按鈕
    m_PlayButton = std::make_shared<ImageTextButton>("Play", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_PlayButton->m_Transform.translation = {playX, playY};
    m_Root->AddChild(m_PlayButton);
    for (auto& part : m_PlayButton->GetParts()) {
        m_Root->AddChild(part);
    }
}

void StartScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    m_PlayButton->Update();
}

void StartScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_PlayButton.reset();
    m_BackgroundImage.reset();
    m_BackgroundObject.reset();
}
