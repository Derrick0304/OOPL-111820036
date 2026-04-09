#include "Scene/StageSelectScene.hpp"

#include "App.hpp"
#include "Scene/BattleScene.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Stage/StageLoader.hpp"
#include "UI/TextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

StageSelectScene::StageSelectScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void StageSelectScene::Enter() {
    LOG_INFO("Entering StageSelectScene");

    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/StageSelect.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.28f, 1.28f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 30.0f};
    m_Root->AddChild(m_BackgroundObject);

    m_PanelImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_PanelObject = std::make_shared<Util::GameObject>(m_PanelImage, 5.0f);
    m_PanelObject->m_Transform.scale = {6.0f, 4.2f};
    m_PanelObject->m_Transform.translation = {0.0f, 0.0f};
    m_Root->AddChild(m_PanelObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, "Select Stage", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {0.0f, 170.0f};
    m_Root->AddChild(m_TitleObject);

    m_Stages = StageLoader::GetAllStages();
    BuildStageButtons();

    if (m_Stages.empty()) {
        m_MessageText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 28, "No stages found in Resources/Data/Stages.json");
        m_MessageObject = std::make_shared<Util::GameObject>(m_MessageText, 15.0f);
        m_MessageObject->m_Transform.translation = {0.0f, 90.0f};
        m_Root->AddChild(m_MessageObject);
    }

    m_BackButton = std::make_shared<TextButton>("Back", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    });
    m_BackButton->SetTextColor(Util::Color(255, 255, 255), Util::Color(255, 235, 120));
    m_BackButton->m_Transform.translation = {0.0f, -180.0f};
    m_Root->AddChild(m_BackButton);
}

void StageSelectScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    for (auto& button : m_StageButtons) {
        button->Update();
    }
    if (m_BackButton) {
        m_BackButton->Update();
    }
}

void StageSelectScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_StageButtons.clear();
}

void StageSelectScene::BuildStageButtons() {
    m_StageButtons.clear();

    float startY = 90.0f;
    const float spacingY = 55.0f;
    for (size_t i = 0; i < m_Stages.size(); ++i) {
        const StageData stage = m_Stages[i];
        auto button = std::make_shared<TextButton>(stage.displayName + " (" + stage.id + ")", [this, stage]() {
            m_App.ChangeScene(std::make_unique<BattleScene>(m_App, stage));
        }, 30);

        button->SetTextColor(Util::Color(255, 255, 255), Util::Color(255, 235, 120));
        button->m_Transform.translation = {0.0f, startY - static_cast<float>(i) * spacingY};
        m_StageButtons.push_back(button);
        m_Root->AddChild(button);
    }
}
