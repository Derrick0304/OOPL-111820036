#include "Scene/ChapterSelectScene.hpp"

#include "App.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "UI/TextButton.hpp"
#include "UI/ImageTextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ChapterSelectScene::ChapterSelectScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void ChapterSelectScene::Enter() {
    LOG_INFO("Entering ChapterSelectScene");

    float titleX = 0.0f, titleY = 250.0f;
    float ch1X = 0.0f, ch1Y = 100.0f;
    float ch2X = 0.0f, ch2Y = 0.0f;
    float ch3X = 0.0f, ch3Y = -100.0f;
    float backX = -538.0f, backY = -303.0f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("ChapterSelectScene")) {
                auto& scene = layout["ChapterSelectScene"];
                if (scene.contains("TitleText")) {
                    titleX = scene["TitleText"]["x"].get<float>();
                    titleY = scene["TitleText"]["y"].get<float>();
                }
                if (scene.contains("Chapter1Button")) {
                    ch1X = scene["Chapter1Button"]["x"].get<float>();
                    ch1Y = scene["Chapter1Button"]["y"].get<float>();
                }
                if (scene.contains("Chapter2Button")) {
                    ch2X = scene["Chapter2Button"]["x"].get<float>();
                    ch2Y = scene["Chapter2Button"]["y"].get<float>();
                }
                if (scene.contains("Chapter3Button")) {
                    ch3X = scene["Chapter3Button"]["x"].get<float>();
                    ch3Y = scene["Chapter3Button"]["y"].get<float>();
                }
                if (scene.contains("BackButton")) {
                    backX = scene["BackButton"]["x"].get<float>();
                    backY = scene["BackButton"]["y"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in ChapterSelectScene: {}", e.what());
        }
    }

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/StageSelect.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.28f, 1.28f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 30.0f};
    m_Root->AddChild(m_BackgroundObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, "Select Chapter", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_TitleObject);

    m_Chapter1Button = std::make_shared<ImageTextButton>("Ch1: The Battle Cats Rising", [this]() {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 1));
    }, ImageTextButton::Type::LONG);
    m_Chapter1Button->m_Transform.translation = {ch1X, ch1Y};
    m_Root->AddChild(m_Chapter1Button);
    for (auto& part : m_Chapter1Button->GetParts()) m_Root->AddChild(part);

    m_Chapter2Button = std::make_shared<ImageTextButton>("Ch2: The Emperor of Darkness", [this]() {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 2));
    }, ImageTextButton::Type::LONG);
    m_Chapter2Button->m_Transform.translation = {ch2X, ch2Y};
    m_Root->AddChild(m_Chapter2Button);
    for (auto& part : m_Chapter2Button->GetParts()) m_Root->AddChild(part);

    m_Chapter3Button = std::make_shared<ImageTextButton>("Ch3: Revival of Bahamut-Cat", [this]() {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 3));
    }, ImageTextButton::Type::LONG);
    m_Chapter3Button->m_Transform.translation = {ch3X, ch3Y};
    m_Root->AddChild(m_Chapter3Button);
    for (auto& part : m_Chapter3Button->GetParts()) m_Root->AddChild(part);

    m_BackButton = std::make_shared<ImageTextButton>("Back", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) m_Root->AddChild(part);
}

void ChapterSelectScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    m_Chapter1Button->Update();
    m_Chapter2Button->Update();
    m_Chapter3Button->Update();
    m_BackButton->Update();
}

void ChapterSelectScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
}
