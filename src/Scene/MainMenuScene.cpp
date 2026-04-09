#include "Scene/MainMenuScene.hpp"

#include "App.hpp"
#include "Scene/StageSelectScene.hpp"
#include "UI/TextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

MainMenuScene::MainMenuScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void MainMenuScene::Enter() {
    LOG_INFO("Entering MainMenuScene");

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/MainMenu.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.4f, 1.4f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 40.0f};
    m_Root->AddChild(m_BackgroundObject);

    m_PanelImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_PanelObject = std::make_shared<Util::GameObject>(m_PanelImage, 5.0f);
    m_PanelObject->m_Transform.scale = {4.8f, 3.2f};
    m_PanelObject->m_Transform.translation = {0.0f, -20.0f};
    m_Root->AddChild(m_PanelObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 56, "Battle Cats", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {0.0f, 70.0f};
    m_Root->AddChild(m_TitleObject);

    m_StartButton = std::make_shared<TextButton>("Start", [this]() {
        m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App));
    });
    m_StartButton->SetTextColor(Util::Color(255, 255, 255), Util::Color(255, 235, 120));
    m_StartButton->m_Transform.translation = {0.0f, -40.0f};
    m_Root->AddChild(m_StartButton);

    m_ExitButton = std::make_shared<TextButton>("Exit", [this]() {
        m_App.RequestExit();
    });
    m_ExitButton->SetTextColor(Util::Color(255, 255, 255), Util::Color(255, 235, 120));
    m_ExitButton->m_Transform.translation = {0.0f, -110.0f};
    m_Root->AddChild(m_ExitButton);
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
