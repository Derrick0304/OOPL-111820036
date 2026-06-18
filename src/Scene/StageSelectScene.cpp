#include "Scene/StageSelectScene.hpp"

#include "App.hpp"
#include "Scene/BattleScene.hpp"
#include "Scene/ChapterSelectScene.hpp"
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

StageSelectScene::StageSelectScene(App& app, int chapterId)
    : m_App(app), m_ChapterId(chapterId), m_Root(std::make_shared<Util::GameObject>()) {}

void StageSelectScene::Enter() {
    LOG_INFO("Entering StageSelectScene for Chapter {}", m_ChapterId);

    float titleX = 0.0f, titleY = 250.0f;
    float backX = -500.0f, backY = -300.0f;
    float startBtnX = 500.0f, startBtnY = -300.0f;
    m_CarouselY = 0.0f;
    m_SpacingX = 350.0f;

    float topBorderX = 0.0f, topBorderY = 330.0f, topBorderScaleX = 1.87f, topBorderScaleY = 1.87f;
    float bottomBorderX = 0.0f, bottomBorderY = -330.0f, bottomBorderScaleX = 1.87f, bottomBorderScaleY = 1.87f;
    float stageSelectTitleX = -480.0f, stageSelectTitleY = 330.0f;
    int stageSelectTitleFontSize = 32;
    float xpX = 540.0f, xpY = 330.0f;
    int xpFontSize = 24;
    float catFoodX = 580.0f, catFoodY = -330.0f;
    int catFoodFontSize = 24;

    float energyBarX = 430.0f, energyBarY = -70.0f, energyBarScaleX = 1.5f, energyBarScaleY = 1.5f;
    float energyTextX = 485.0f, energyTextY = -70.0f;
    int energyTextFontSize = 20;

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
                if (scene.contains("StageSelectTitle")) {
                    stageSelectTitleX = scene["StageSelectTitle"]["x"].get<float>();
                    stageSelectTitleY = scene["StageSelectTitle"]["y"].get<float>();
                    stageSelectTitleFontSize = scene["StageSelectTitle"]["fontSize"].get<int>();
                }
                if (scene.contains("XPDisplay")) {
                    xpX = scene["XPDisplay"]["x"].get<float>();
                    xpY = scene["XPDisplay"]["y"].get<float>();
                    xpFontSize = scene["XPDisplay"]["fontSize"].get<int>();
                }
                if (scene.contains("CatFoodDisplay")) {
                    catFoodX = scene["CatFoodDisplay"]["x"].get<float>();
                    catFoodY = scene["CatFoodDisplay"]["y"].get<float>();
                    catFoodFontSize = scene["CatFoodDisplay"]["fontSize"].get<int>();
                }
                if (scene.contains("EnergyBar")) {
                    energyBarX = scene["EnergyBar"]["x"].get<float>();
                    energyBarY = scene["EnergyBar"]["y"].get<float>();
                    energyBarScaleX = scene["EnergyBar"]["scaleX"].get<float>();
                    energyBarScaleY = scene["EnergyBar"]["scaleY"].get<float>();
                }
                if (scene.contains("EnergyText")) {
                    energyTextX = scene["EnergyText"]["x"].get<float>();
                    energyTextY = scene["EnergyText"]["y"].get<float>();
                    energyTextFontSize = scene["EnergyText"]["fontSize"].get<int>();
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

    std::string titleStr = "Chapter " + std::to_string(m_ChapterId) + " Stages";
    m_TitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 48, titleStr, Util::Color(255, 255, 255));
    m_TitleObject = std::make_shared<Util::GameObject>(m_TitleText, 15.0f);
    m_TitleObject->m_Transform.translation = {titleX, titleY};
    m_Root->AddChild(m_TitleObject);

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

    // 建立頂部左上角選關英文標題
    m_StageSelectTitleText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", stageSelectTitleFontSize, "Stage Select", Util::Color(255, 240, 0));
    m_StageSelectTitleObject = std::make_shared<Util::GameObject>(m_StageSelectTitleText, 19.0f);
    m_StageSelectTitleObject->m_Transform.translation = {stageSelectTitleX, stageSelectTitleY};
    m_Root->AddChild(m_StageSelectTitleObject);

    // 建立頂部 XP 顯示
    std::string xpStr = "XP  " + std::to_string(m_App.GetTotalXP());
    m_XPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", xpFontSize, xpStr, Util::Color(0, 0, 0));
    m_XPObject = std::make_shared<Util::GameObject>(m_XPText, 19.0f);
    m_XPObject->m_Transform.translation = {xpX, xpY};
    m_Root->AddChild(m_XPObject);

    // 建立底部貓罐頭顯示
    std::string catFoodStr = "Cat Food  " + std::to_string(m_App.GetCatFood());
    m_CatFoodText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", catFoodFontSize, catFoodStr, Util::Color(0, 0, 0));
    m_CatFoodObject = std::make_shared<Util::GameObject>(m_CatFoodText, 19.0f);
    m_CatFoodObject->m_Transform.translation = {catFoodX, catFoodY};
    m_Root->AddChild(m_CatFoodObject);

    // 建立體力條與體力數值顯示
    m_EnergyBarImage = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/UI_EnergyBar.png");
    
    m_EnergyBarObject = std::make_shared<Util::GameObject>(m_EnergyBarImage, 15.0f);
    m_EnergyBarObject->m_Transform.translation = {energyBarX, energyBarY};
    m_EnergyBarObject->m_Transform.scale = {energyBarScaleX, energyBarScaleY};
    m_Root->AddChild(m_EnergyBarObject);

    std::string energyStr = std::to_string(m_App.GetCurrentEnergy());
    m_EnergyValText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", energyTextFontSize, energyStr, Util::Color(255, 255, 255));
    m_EnergyValObject = std::make_shared<Util::GameObject>(m_EnergyValText, 16.0f);
    m_EnergyValObject->m_Transform.translation = {energyTextX, energyTextY};
    m_Root->AddChild(m_EnergyValObject);

    auto allStages = StageLoader::GetAllStages();
    m_Stages.clear();
    for (const auto& stage : allStages) {
        if (stage.chapterId == m_ChapterId) {
            m_Stages.push_back(stage);
        }
    }
    
    m_StageItems.clear();

    for (size_t i = 0; i < m_Stages.size(); ++i) {
        // 卡片底圖 (目前保留作為背景或透明點擊區)
        auto baseImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/BlackMask.png");
        auto baseObj = std::make_shared<Util::GameObject>(baseImg, 10.0f);
        baseObj->SetVisible(false); // 暫時隱藏，因為我們有了 Header
        
        // 標題框 (StageCard_Header)
        auto headerImg = std::make_shared<Util::Image>(RESOURCE_DIR"/UI/Buttons/StageCard_Header.png");
        auto headerObj = std::make_shared<Util::GameObject>(headerImg, 11.0f);

        // 關卡名稱文字 (置於 Header 內) - 縮小字體至 22
        auto txt = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 22, m_Stages[i].displayName, Util::Color(0, 0, 0));
        auto txtObj = std::make_shared<Util::GameObject>(txt, 15.0f);
        
        // 體力消耗文字 - 縮小字體至 16
        std::string energyStr = "Energy  -" + std::to_string(m_Stages[i].cost);
        auto energyTxt = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 16, energyStr, Util::Color(0, 0, 0));
        auto energyTxtObj = std::make_shared<Util::GameObject>(energyTxt, 15.0f);

        // 通關次數文字 - 縮小字體至 16
        auto clearedTxt = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 16, "Cleared  0", Util::Color(0, 0, 0));
        auto clearedTxtObj = std::make_shared<Util::GameObject>(clearedTxt, 15.0f);

        m_Root->AddChild(baseObj);
        m_Root->AddChild(headerObj);
        m_Root->AddChild(txtObj);
        m_Root->AddChild(energyTxtObj);
        m_Root->AddChild(clearedTxtObj);
        
        m_StageItems.push_back({baseObj, headerObj, txtObj, energyTxtObj, clearedTxtObj, m_Stages[i]});
    }

    m_BackButton = std::make_shared<ImageTextButton>(" ", [this]() {
        m_App.ChangeScene(std::make_unique<ChapterSelectScene>(m_App));
    }, "/UI/Buttons/Btn_Back_Circle.png"); // 這裡不傳入自定義黃/紫邊框，將自動使用預設
    m_BackButton->SetFlashEnabled(false); // 圓形按鈕不需要閃爍外框
    m_BackButton->m_Transform.translation = {backX, backY};
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) m_Root->AddChild(part);

    m_StartButton = std::make_shared<ImageTextButton>(" ", [this]() {
        if (m_CurrentIndex >= 0 && m_CurrentIndex < m_Stages.size()) {
            int cost = m_Stages[m_CurrentIndex].cost;
            if (m_App.GetCurrentEnergy() >= cost) {
                m_App.SetCurrentEnergy(m_App.GetCurrentEnergy() - cost);
                m_App.ChangeScene(std::make_unique<BattleScene>(m_App, m_Stages[m_CurrentIndex]));
            } else {
                LOG_WARN("Not enough energy! Cost: {}, Current: {}", cost, m_App.GetCurrentEnergy());
                ShowEnergyWarning();
            }
        }
    }, "/UI/Buttons/Btn_Attack_Base.png");
    m_StartButton->m_Transform.translation = {startBtnX, startBtnY};
    m_Root->AddChild(m_StartButton);
    for (auto& part : m_StartButton->GetParts()) m_Root->AddChild(part);

    // 建立體力不足警告提示 (居中，紅色)
    m_WarningText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 36, "Not Enough Energy!", Util::Color(255, 0, 0));
    m_WarningObject = std::make_shared<Util::GameObject>(m_WarningText, 25.0f);
    m_WarningObject->m_Transform.translation = {0.0f, 0.0f};
    m_WarningObject->SetVisible(false);
    m_Root->AddChild(m_WarningObject);

    // 初始化捲動位置，對齊到目前索引
    m_CurrentScrollX = -m_CurrentIndex * m_SpacingX;
    m_TargetScrollX = m_CurrentScrollX;
}

void StageSelectScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<ChapterSelectScene>(m_App));
        return;
    }

    HandleInput();
    UpdateCarousel(dt);

    m_BackButton->Update();
    m_StartButton->Update();

    // 動態更新 XP 與貓罐頭的文字顯示
    if (m_XPText) {
        m_XPText->SetText("XP  " + std::to_string(m_App.GetTotalXP()));
    }
    if (m_CatFoodText) {
        m_CatFoodText->SetText("Cat Food  " + std::to_string(m_App.GetCatFood()));
    }
    if (m_EnergyValText) {
        m_EnergyValText->SetText(std::to_string(m_App.GetCurrentEnergy()));
    }

    // 更新體力警告計時器
    if (m_WarningTimer > 0.0f) {
        m_WarningTimer -= dt;
        if (m_WarningTimer <= 0.0f && m_WarningObject) {
            m_WarningObject->SetVisible(false);
        }
    }
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
        float dist = std::abs(x);
        float scale = 1.0f;
        if (dist < m_SpacingX) {
            scale = 1.0f + 0.3f * (1.0f - dist / m_SpacingX);
        }

        // 設置各子元件的位置 (依 Y 軸層級錯開)與縮放
        m_StageItems[i].base->m_Transform.translation = {x, m_CarouselY};
        m_StageItems[i].base->m_Transform.scale = {2.5f * scale, 1.5f * scale};

        m_StageItems[i].header->m_Transform.translation = {x, m_CarouselY + 60.0f * scale};
        m_StageItems[i].header->m_Transform.scale = {scale, scale};

        m_StageItems[i].textObj->m_Transform.translation = {x, m_CarouselY + 60.0f * scale};
        m_StageItems[i].textObj->m_Transform.scale = {scale, scale};

        m_StageItems[i].energyTxtObj->m_Transform.translation = {x, m_CarouselY - 10.0f * scale};
        m_StageItems[i].energyTxtObj->m_Transform.scale = {scale, scale};

        m_StageItems[i].clearedTxtObj->m_Transform.translation = {x, m_CarouselY - 40.0f * scale};
        m_StageItems[i].clearedTxtObj->m_Transform.scale = {scale, scale};
        
        if (!m_IsDragging && dist < m_SpacingX * 0.5f) {
            m_CurrentIndex = i;
        }
    }
}

void StageSelectScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_StageItems.clear();

    m_TopBorder.reset();
    m_BottomBorder.reset();
    m_BorderImage.reset();
    m_StageSelectTitleText.reset();
    m_StageSelectTitleObject.reset();
    m_XPText.reset();
    m_XPObject.reset();
    m_CatFoodText.reset();
    m_CatFoodObject.reset();
    m_EnergyBarImage.reset();
    m_EnergyBarObject.reset();
    m_EnergyValText.reset();
    m_EnergyValObject.reset();
    m_WarningText.reset();
    m_WarningObject.reset();
}

void StageSelectScene::ShowEnergyWarning() {
    if (m_WarningObject) {
        m_WarningObject->SetVisible(true);
        m_WarningTimer = 2.0f;
    }
}
