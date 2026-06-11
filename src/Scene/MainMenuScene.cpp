#include "Scene/MainMenuScene.hpp"

#include "App.hpp"
#include "Scene/ChapterSelectScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "UI/TextButton.hpp"
#include "UI/ImageTextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

MainMenuScene::MainMenuScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void MainMenuScene::Enter() {
    LOG_INFO("Entering MainMenuScene");

    float titleX = 0.0f, titleY = 70.0f;
    float startX = 0.0f, startY = -40.0f;
    float exitX = 0.0f, exitY = -110.0f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("MainMenuScene")) {
                auto& scene = layout["MainMenuScene"];
                if (scene.contains("TitleText")) {
                    titleX = scene["TitleText"]["x"].get<float>();
                    titleY = scene["TitleText"]["y"].get<float>();
                }
                if (scene.contains("StartButton")) {
                    startX = scene["StartButton"]["x"].get<float>();
                    startY = scene["StartButton"]["y"].get<float>();
                }
                if (scene.contains("ExitButton")) {
                    exitX = scene["ExitButton"]["x"].get<float>();
                    exitY = scene["ExitButton"]["y"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in MainMenuScene: {}", e.what());
        }
    }

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/MainMenu.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.4f, 1.4f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 40.0f};
    m_Root->AddChild(m_BackgroundObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 56, "Battle Cats", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_TitleObject);

    m_StartButton = std::make_shared<ImageTextButton>("Start", [this]() {
        m_App.ChangeScene(std::make_unique<ChapterSelectScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_StartButton->m_Transform.translation = {startX, startY};
    m_Root->AddChild(m_StartButton);
    for (auto& part : m_StartButton->GetParts()) {
        m_Root->AddChild(part);
    }

    m_ExitButton = std::make_shared<ImageTextButton>("Exit", [this]() {
        m_App.RequestExit();
    }, ImageTextButton::Type::LONG);
    m_ExitButton->m_Transform.translation = {exitX, exitY};
    m_Root->AddChild(m_ExitButton);
    for (auto& part : m_ExitButton->GetParts()) {
        m_Root->AddChild(part);
    }
}

void MainMenuScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    m_StartButton->Update();
    m_ExitButton->Update();
}

void MainMenuScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
}
