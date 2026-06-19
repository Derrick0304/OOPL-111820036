#include "Scene/MainMenuScene.hpp"

#include "App.hpp"
#include "Scene/StartScene.hpp"
#include "Scene/ChapterSelectScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/EquipScene.hpp"
#include "Scene/UpgradeScene.hpp"
#include "UI/TextButton.hpp"
#include "UI/ImageTextButton.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

MainMenuScene::MainMenuScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void MainMenuScene::Enter() {
    LOG_INFO("Entering MainMenuScene");

    float startX = -350.0f, startY = 120.0f;
    float upgradeX = -350.0f, upgradeY = 10.0f;
    float equipX = -350.0f, equipY = -100.0f;
    float backX = -560.0f, backY = -290.0f;

    float topBorderX = 0.0f, topBorderY = 330.0f;
    float topBorderScaleX = 1.87f, topBorderScaleY = 1.87f;
    float bottomBorderX = 0.0f, bottomBorderY = -330.0f;
    float bottomBorderScaleX = 1.87f, bottomBorderScaleY = 1.87f;

    float xpX = 530.0f, xpY = 330.0f;
    int xpFontSize = 24;
    float xpIconX = 460.0f, xpIconY = 330.0f;
    float xpIconScaleX = 1.87f, xpIconScaleY = 1.87f;

    float catFoodX = 530.0f, catFoodY = -330.0f;
    int catFoodFontSize = 24;
    float catFoodIconX = 460.0f, catFoodIconY = -330.0f;
    float catFoodIconScaleX = 1.87f, catFoodIconScaleY = 1.87f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("MainMenuScene")) {
                auto& scene = layout["MainMenuScene"];
                if (scene.contains("StartButton")) {
                    startX = scene["StartButton"]["x"].get<float>();
                    startY = scene["StartButton"]["y"].get<float>();
                }
                if (scene.contains("UpgradeButton")) {
                    upgradeX = scene["UpgradeButton"]["x"].get<float>();
                    upgradeY = scene["UpgradeButton"]["y"].get<float>();
                }
                if (scene.contains("EquipButton")) {
                    equipX = scene["EquipButton"]["x"].get<float>();
                    equipY = scene["EquipButton"]["y"].get<float>();
                }
                if (scene.contains("BackButton")) {
                    backX = scene["BackButton"]["x"].get<float>();
                    backY = scene["BackButton"]["y"].get<float>();
                }
                if (scene.contains("TopBorder")) {
                    topBorderX = scene["TopBorder"]["x"].get<float>();
                    topBorderY = scene["TopBorder"]["y"].get<float>();
                    topBorderScaleX = scene["TopBorder"]["scaleX"].get<float>();
                    topBorderScaleY = scene["TopBorder"]["scaleY"].get<float>();
                }
                if (scene.contains("BottomBorder")) {
                    bottomBorderX = scene["BottomBorder"]["x"].get<float>();
                    bottomBorderY = scene["BottomBorder"]["y"].get<float>();
                    bottomBorderScaleX = scene["BottomBorder"]["scaleX"].get<float>();
                    bottomBorderScaleY = scene["BottomBorder"]["scaleY"].get<float>();
                }
                if (scene.contains("XPDisplay")) {
                    xpX = scene["XPDisplay"]["x"].get<float>();
                    xpY = scene["XPDisplay"]["y"].get<float>();
                    xpFontSize = scene["XPDisplay"]["fontSize"].get<int>();
                }
                if (scene.contains("XPIcon")) {
                    xpIconX = scene["XPIcon"]["x"].get<float>();
                    xpIconY = scene["XPIcon"]["y"].get<float>();
                    xpIconScaleX = scene["XPIcon"]["scaleX"].get<float>();
                    xpIconScaleY = scene["XPIcon"]["scaleY"].get<float>();
                }
                if (scene.contains("CatFoodDisplay")) {
                    catFoodX = scene["CatFoodDisplay"]["x"].get<float>();
                    catFoodY = scene["CatFoodDisplay"]["y"].get<float>();
                    catFoodFontSize = scene["CatFoodDisplay"]["fontSize"].get<int>();
                }
                if (scene.contains("CatFoodIcon")) {
                    catFoodIconX = scene["CatFoodIcon"]["x"].get<float>();
                    catFoodIconY = scene["CatFoodIcon"]["y"].get<float>();
                    catFoodIconScaleX = scene["CatFoodIcon"]["scaleX"].get<float>();
                    catFoodIconScaleY = scene["CatFoodIcon"]["scaleY"].get<float>();
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

    // 建立頂部與底部邊框
    m_BorderImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/UI_Border.png");
    
    m_TopBorder = std::make_shared<Util::GameObject>(m_BorderImage, 18.0f);
    m_TopBorder->m_Transform.translation = {topBorderX, topBorderY};
    m_TopBorder->m_Transform.scale = {topBorderScaleX, topBorderScaleY};
    m_Root->AddChild(m_TopBorder);

    m_BottomBorder = std::make_shared<Util::GameObject>(m_BorderImage, 18.0f);
    m_BottomBorder->m_Transform.translation = {bottomBorderX, bottomBorderY};
    m_BottomBorder->m_Transform.scale = {bottomBorderScaleX, -bottomBorderScaleY};
    m_Root->AddChild(m_BottomBorder);

    // 建立頂部 XP 圖示與數值顯示
    m_XPIconImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/UI_XPIcon.png");
    m_XPIconObject = std::make_shared<Util::GameObject>(m_XPIconImage, 19.0f);
    m_XPIconObject->m_Transform.translation = {xpIconX, xpIconY};
    m_XPIconObject->m_Transform.scale = {xpIconScaleX, xpIconScaleY};
    m_Root->AddChild(m_XPIconObject);

    std::string xpStr = std::to_string(m_App.GetTotalXP());
    m_XPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", xpFontSize, xpStr, Util::Color(0, 0, 0));
    m_XPObject = std::make_shared<Util::GameObject>(m_XPText, 19.0f);
    m_XPObject->m_Transform.translation = {xpX, xpY};
    m_Root->AddChild(m_XPObject);

    // 建立底部貓罐頭圖示與數值顯示
    m_CatFoodIconImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/UI_CatFoodIcon.png");
    m_CatFoodIconObject = std::make_shared<Util::GameObject>(m_CatFoodIconImage, 19.0f);
    m_CatFoodIconObject->m_Transform.translation = {catFoodIconX, catFoodIconY};
    m_CatFoodIconObject->m_Transform.scale = {catFoodIconScaleX, catFoodIconScaleY};
    m_Root->AddChild(m_CatFoodIconObject);

    std::string catFoodStr = std::to_string(m_App.GetCatFood());
    m_CatFoodText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", catFoodFontSize, catFoodStr, Util::Color(0, 0, 0));
    m_CatFoodObject = std::make_shared<Util::GameObject>(m_CatFoodText, 19.0f);
    m_CatFoodObject->m_Transform.translation = {catFoodX, catFoodY};
    m_Root->AddChild(m_CatFoodObject);

    // 按鈕：Start!!
    m_StartButton = std::make_shared<ImageTextButton>("Start!!", [this]() {
        m_App.ChangeScene(std::make_unique<ChapterSelectScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_StartButton->m_Transform.translation = {startX, startY};
    m_Root->AddChild(m_StartButton);
    for (auto& part : m_StartButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 按鈕：Upgrade
    m_UpgradeButton = std::make_shared<ImageTextButton>("Upgrade", [this]() {
        m_App.ChangeScene(std::make_unique<UpgradeScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_UpgradeButton->m_Transform.translation = {upgradeX, upgradeY};
    m_Root->AddChild(m_UpgradeButton);
    for (auto& part : m_UpgradeButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 按鈕：Equip
    m_EquipButton = std::make_shared<ImageTextButton>("Equip", [this]() {
        m_App.ChangeScene(std::make_unique<EquipScene>(m_App));
    }, ImageTextButton::Type::LONG);
    m_EquipButton->m_Transform.translation = {equipX, equipY};
    m_Root->AddChild(m_EquipButton);
    for (auto& part : m_EquipButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 按鈕：返回
    m_BackButton = std::make_shared<ImageTextButton>(" ", [this]() {
        m_App.ChangeScene(std::make_unique<StartScene>(m_App));
    }, "/UI/Buttons/Btn_Back_Circle.png");
    m_BackButton->SetFlashEnabled(false);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_BackButton->SetZIndex(20.0f);
    m_Root->AddChild(m_BackButton);
}

void MainMenuScene::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_App.RequestExit();
        return;
    }

    m_StartButton->Update();
    m_UpgradeButton->Update();
    m_EquipButton->Update();
    m_BackButton->Update();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 cursor = Util::Input::GetCursorPosition();
        auto isClickOn = [](const glm::vec2& cur, std::shared_ptr<Util::GameObject> obj, float w, float h) {
            if (!obj) return false;
            glm::vec2 pos = obj->m_Transform.translation;
            return cur.x >= pos.x - w / 2.0f && cur.x <= pos.x + w / 2.0f &&
                   cur.y >= pos.y - h / 2.0f && cur.y <= pos.y + h / 2.0f;
        };

        if (isClickOn(cursor, m_XPIconObject, 150.0f, 50.0f)) {
            if (m_App.GetCatFood() >= 100) {
                m_App.AddCatFood(-100);
                m_App.AddXP(10000);
                LOG_INFO("Exchanged 100 Cat Food for 10000 XP");
            }
        } else if (isClickOn(cursor, m_CatFoodIconObject, 150.0f, 50.0f)) {
            m_App.AddCatFood(1000);
            LOG_INFO("Cheat: Added 1000 Cat Food");
        }
    }

    if (m_XPText) {
        m_XPText->SetText(std::to_string(m_App.GetTotalXP()));
    }
    if (m_CatFoodText) {
        m_CatFoodText->SetText(std::to_string(m_App.GetCatFood()));
    }
}

void MainMenuScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_StartButton.reset();
    m_UpgradeButton.reset();
    m_EquipButton.reset();
    m_BackButton.reset();
    m_TopBorder.reset();
    m_BottomBorder.reset();
    m_XPIconObject.reset();
    m_XPText.reset();
    m_XPObject.reset();
    m_CatFoodIconObject.reset();
    m_CatFoodText.reset();
    m_CatFoodObject.reset();
    m_BackgroundImage.reset();
    m_BackgroundObject.reset();
}
