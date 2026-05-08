#include "Scene/StageSelectScene.hpp"

#include "App.hpp"
#include "Scene/BattleScene.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Stage/StageLoader.hpp"
#include "UI/TextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

StageSelectScene::StageSelectScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void StageSelectScene::Enter() {
    LOG_INFO("Entering StageSelectScene");

    float titleX = 0.0f, titleY = 170.0f;
    float backX = 0.0f, backY = -180.0f;
    float listX = 0.0f, listY = 90.0f;
    float listSpacingY = 55.0f;
    float panelX = 0.0f, panelY = 0.0f;
    float panelScaleX = 6.0f, panelScaleY = 4.2f;

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
                if (scene.contains("StageList")) {
                    listX = scene["StageList"]["x"].get<float>();
                    listY = scene["StageList"]["y"].get<float>();
                    if (scene["StageList"].contains("spacingY")) listSpacingY = scene["StageList"]["spacingY"].get<float>();
                }
                if (scene.contains("Panel")) {
                    panelX = scene["Panel"]["x"].get<float>();
                    panelY = scene["Panel"]["y"].get<float>();
                    if (scene["Panel"].contains("scaleX")) panelScaleX = scene["Panel"]["scaleX"].get<float>();
                    if (scene["Panel"].contains("scaleY")) panelScaleY = scene["Panel"]["scaleY"].get<float>();
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

    m_PanelImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
    m_PanelObject = std::make_shared<Util::GameObject>(m_PanelImage, 5.0f);
    m_PanelObject->m_Transform.scale = {panelScaleX, panelScaleY};
    m_PanelObject->m_Transform.translation = {panelX, panelY};
    m_Root->AddChild(m_PanelObject);

    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, "Select Stage", Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_TitleObject);

    m_Stages = StageLoader::GetAllStages();
    
    m_StageButtons.clear();
    for (size_t i = 0; i < m_Stages.size(); ++i) {
        const StageData stage = m_Stages[i];
        auto button = std::make_shared<ImageTextButton>(stage.displayName, [this, stage]() {
            m_App.ChangeScene(std::make_unique<BattleScene>(m_App, stage));
        }, ImageTextButton::Type::LONG);

        button->m_Transform.translation = {listX, listY - static_cast<float>(i) * listSpacingY};
        m_StageButtons.push_back(button);
        m_Root->AddChild(button);
        for (auto& part : button->GetParts()) {
            m_Root->AddChild(part);
        }
    }

    if (m_Stages.empty()) {
        m_MessageText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 28, "No stages found in Resources/Data/Stages.json");
        m_MessageObject = std::make_shared<Util::GameObject>(m_MessageText, 15.0f);
        m_MessageObject->m_Transform.translation = {0.0f, 90.0f};
        m_Root->AddChild(m_MessageObject);
    }

    m_BackButton = std::make_shared<ImageTextButton>("Back", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) {
        m_Root->AddChild(part);
    }
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
