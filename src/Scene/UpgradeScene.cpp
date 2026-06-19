#include "Scene/UpgradeScene.hpp"

#include "App.hpp"
#include "Scene/MainMenuScene.hpp"
#include "UnitFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>

using json = nlohmann::json;

UpgradeScene::UpgradeScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void UpgradeScene::Enter() {
    LOG_INFO("Entering UpgradeScene");

    float backX = -560.0f, backY = -290.0f;
    float upgradeX = -420.0f, upgradeY = -160.0f;
    float xpX = 460.0f, xpY = 322.0f;
    int xpFontSize = 24;
    float catFoodX = 460.0f, catFoodY = -332.0f;
    int catFoodFontSize = 24;

    m_ListY = 37.0f;
    m_SpacingX = 180.0f;

    // 從 UI_Layout.json 讀取配置
    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("UpgradeScene")) {
                auto& scene = layout["UpgradeScene"];
                if (scene.contains("BackButton")) {
                    backX = scene["BackButton"]["x"].get<float>();
                    backY = scene["BackButton"]["y"].get<float>();
                }
                if (scene.contains("UpgradeButton")) {
                    upgradeX = scene["UpgradeButton"]["x"].get<float>();
                    upgradeY = scene["UpgradeButton"]["y"].get<float>();
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
                if (scene.contains("CatListScroll")) {
                    m_ListY = scene["CatListScroll"]["y"].get<float>();
                    m_SpacingX = scene["CatListScroll"]["spacingX"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in UpgradeScene: {}", e.what());
        }
    }

    // 1. 載入背景圖
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/UpgradeSceneBg.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {0.6598f, 0.6598f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 0.0f};
    m_Root->AddChild(m_BackgroundObject);

    // 2. 建立返回按鈕
    m_BackButton = std::make_shared<ImageTextButton>(" ", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, "/UI/Buttons/Btn_Back_Circle.png");
    m_BackButton->SetFlashEnabled(false);
    m_BackButton->SetBaseScale(2.0f);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_BackButton->SetZIndex(20.0f);
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 3. 建立升級按鈕
    m_UpgradeButton = std::make_shared<ImageTextButton>("Upgrade", [this]() {
        if (m_SelectedIndex < 0 || m_SelectedIndex >= static_cast<int>(m_AvailableCats.size())) return;
        std::string catId = m_AvailableCats[m_SelectedIndex];
        int lvl = m_App.GetCatLevel(catId);
        if (lvl < 10) {
            int cost = lvl * 1000;
            if (m_App.GetTotalXP() >= cost) {
                m_App.UpgradeCat(catId);
                m_App.SetTotalXP(m_App.GetTotalXP() - cost);
                LOG_INFO("Upgraded {} to level {}", catId, lvl + 1);
                UpdateSelectedInfo();
            } else {
                LOG_INFO("Not enough XP to upgrade {}", catId);
            }
        }
    }, ImageTextButton::Type::LONG);
    m_UpgradeButton->SetBaseScale(1.8f);
    m_UpgradeButton->m_Transform.translation = {upgradeX, upgradeY};
    m_UpgradeButton->SetZIndex(20.0f);
    m_Root->AddChild(m_UpgradeButton);
    for (auto& part : m_UpgradeButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 4. 動態顯示 XP (右上角覆蓋)
    m_XPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", xpFontSize, std::to_string(m_App.GetTotalXP()), Util::Color(255, 255, 255));
    m_XPObject = std::make_shared<Util::GameObject>(m_XPText, 22.0f);
    m_XPObject->m_Transform.translation = {xpX, xpY};
    m_Root->AddChild(m_XPObject);

    // 5. 動態顯示貓罐頭 (這介面不顯示)
    m_CatFoodText = nullptr;
    m_CatFoodObject = nullptr;

    // 6. 中央預覽立繪物件
    m_CenterPreviewObject = std::make_shared<Util::GameObject>(nullptr, 15.0f);
    m_CenterPreviewObject->SetVisible(false);
    m_Root->AddChild(m_CenterPreviewObject);
    m_LastPreviewedCatId = "";

    // 7. 屬性面板文字初始化
    m_StatsTextName = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 22, " ", Util::Color(255, 255, 255));
    m_StatsObjName = std::make_shared<Util::GameObject>(m_StatsTextName, 21.0f);
    m_StatsObjName->m_Transform.translation = {-260.0f, -250.0f};
    m_Root->AddChild(m_StatsObjName);

    m_StatsTextHpAtk = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 20, " ", Util::Color(255, 220, 180));
    m_StatsObjHpAtk = std::make_shared<Util::GameObject>(m_StatsTextHpAtk, 21.0f);
    m_StatsObjHpAtk->m_Transform.translation = {-20.0f, -250.0f};
    m_Root->AddChild(m_StatsObjHpAtk);

    m_StatsTextCost = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 22, " ", Util::Color(255, 255, 255));
    m_StatsObjCost = std::make_shared<Util::GameObject>(m_StatsTextCost, 21.0f);
    m_StatsObjCost->m_Transform.translation = {260.0f, -250.0f};
    m_Root->AddChild(m_StatsObjCost);

    // 8. 建立可捲動的貓咪列表
    m_AvailableCats = { "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat", "BirdCat", "FishCat", "LizardCat", "TitanCat", "KillerCat" };
    m_CatItems.clear();

    for (size_t i = 0; i < m_AvailableCats.size(); ++i) {
        std::string catId = m_AvailableCats[i];
        auto img = std::make_shared<Util::Image>(RESOURCE_DIR"/Units/Cats/" + catId + "/icon.png");
        auto obj = std::make_shared<Util::GameObject>(img, 10.0f);
        m_Root->AddChild(obj);

        int level = m_App.GetCatLevel(catId);
        auto lvlText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 16, "Lv. " + std::to_string(level), Util::Color(0, 0, 0));
        auto lvlObj = std::make_shared<Util::GameObject>(lvlText, 11.0f);
        m_Root->AddChild(lvlObj);

        m_CatItems.push_back({catId, img, obj, lvlText, lvlObj});
    }

    // 捲動極限與起始位置 (最左或最右拖到中間時停止)
    m_MaxScrollX = 0.0f;
    m_MinScrollX = -static_cast<float>(m_AvailableCats.size() - 1) * m_SpacingX;
    m_SelectedIndex = 0;
    m_CurrentScrollX = 0.0f;
    m_TargetScrollX = 0.0f;

    UpdateSelectedInfo();
}

void UpgradeScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    HandleInput();
    UpdateCarousel(dt);

    m_BackButton->Update();
    m_UpgradeButton->Update();

    // 更新資源數值
    if (m_XPText) {
        m_XPText->SetText(std::to_string(m_App.GetTotalXP()));
    }
}

void UpgradeScene::HandleInput() {
    glm::vec2 cursor = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_MouseDownLastFrame = true;
        m_MouseDownPos = cursor;
        m_LastMouseX = cursor.x;

        // 如果點擊了白色可捲動區域 (Game Y 大約在 -139 到 213)
        if (cursor.y >= -139.0f && cursor.y <= 213.0f) {
            m_IsDragging = true;
        }
    }

    if (m_MouseDownLastFrame) {
        if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
            if (m_IsDragging) {
                float dx = cursor.x - m_LastMouseX;
                m_CurrentScrollX += dx;
                // 限制滾動範圍
                m_CurrentScrollX = std::max(m_MinScrollX, std::min(m_MaxScrollX, m_CurrentScrollX));
                m_TargetScrollX = m_CurrentScrollX;
                m_LastMouseX = cursor.x;
            }
        } else {
            // 滑鼠放開
            m_MouseDownLastFrame = false;
            if (m_IsDragging) {
                m_IsDragging = false;
                
                // 尋找最近的貓咪 icon 置中並 snapping
                int nearestIndex = 0;
                float minDistance = 999999.0f;
                for (size_t i = 0; i < m_CatItems.size(); ++i) {
                    float x = m_CurrentScrollX + i * m_SpacingX;
                    if (std::abs(x) < minDistance) {
                        minDistance = std::abs(x);
                        nearestIndex = i;
                    }
                }
                
                m_SelectedIndex = nearestIndex;
                m_TargetScrollX = -static_cast<float>(m_SelectedIndex) * m_SpacingX;
                UpdateSelectedInfo();
            } else {
                // 單純點擊判定
                float clickDist = glm::distance(m_MouseDownPos, cursor);
                if (clickDist < 10.0f) {
                    // 檢查是否點擊了某個貓咪 icon
                    for (size_t i = 0; i < m_CatItems.size(); ++i) {
                        float x = m_CurrentScrollX + i * m_SpacingX;
                        glm::vec2 itemPos = {x, m_ListY};
                        if (glm::distance(cursor, itemPos) <= 60.0f) {
                            m_SelectedIndex = static_cast<int>(i);
                            m_TargetScrollX = -static_cast<float>(m_SelectedIndex) * m_SpacingX;
                            UpdateSelectedInfo();
                            break;
                        }
                    }
                }
            }
        }
    }
}

void UpgradeScene::UpdateCarousel(float dt) {
    if (!m_IsDragging) {
        m_CurrentScrollX += (m_TargetScrollX - m_CurrentScrollX) * 10.0f * dt;
    }

    for (size_t i = 0; i < m_CatItems.size(); ++i) {
        float x = m_CurrentScrollX + i * m_SpacingX;
        float dist = std::abs(x);
        float scale = 0.85f;

        // 當距離中心越近，圖示越大
        if (dist < m_SpacingX) {
            scale = 0.85f + 0.3f * (1.0f - dist / m_SpacingX);
        }

        m_CatItems[i].object->m_Transform.translation = {x, m_ListY};
        m_CatItems[i].object->m_Transform.scale = {scale, scale};

        // 更新等級顯示
        int level = m_App.GetCatLevel(m_CatItems[i].catId);
        m_CatItems[i].levelText->SetText("Lv. " + std::to_string(level));
        m_CatItems[i].levelObj->m_Transform.translation = {x, m_ListY - 60.0f * scale};
        m_CatItems[i].levelObj->m_Transform.scale = {scale, scale};
    }
}

void UpgradeScene::UpdateSelectedInfo() {
    if (m_SelectedIndex < 0 || m_SelectedIndex >= static_cast<int>(m_AvailableCats.size())) return;
    std::string catId = m_AvailableCats[m_SelectedIndex];
    int level = m_App.GetCatLevel(catId);
    auto baseStats = UnitFactory::Get(catId);

    // 1. 更新中央展示立繪
    if (catId != m_LastPreviewedCatId) {
        m_LastPreviewedCatId = catId;
        auto previewImg = std::make_shared<Util::Image>(RESOURCE_DIR"/Units/Cats/" + catId + "/Walk/0.png");
        m_CenterPreviewObject->SetDrawable(previewImg);
        m_CenterPreviewObject->m_Transform.scale = {2.5f, 2.5f};
        m_CenterPreviewObject->m_Transform.translation = {0.0f, 150.0f};
        m_CenterPreviewObject->SetVisible(true);
    }

    // 2. 計算等級屬性
    float currentHp = baseStats.stats.maxHp * (1.0f + (level - 1) * 0.1f);
    float currentAtk = baseStats.stats.attackDamage * (1.0f + (level - 1) * 0.1f);

    m_StatsTextName->SetText(baseStats.name + "\nLv. " + std::to_string(level));

    if (level < 10) {
        float nextHp = baseStats.stats.maxHp * (1.0f + level * 0.1f);
        float nextAtk = baseStats.stats.attackDamage * (1.0f + level * 0.1f);

        char hpAtkBuf[128];
        snprintf(hpAtkBuf, sizeof(hpAtkBuf), "HP:  %.0f -> %.0f\nATK: %.0f -> %.0f", currentHp, nextHp, currentAtk, nextAtk);
        m_StatsTextHpAtk->SetText(hpAtkBuf);

        int cost = level * 1000;
        m_StatsTextCost->SetText("XP COST:\n" + std::to_string(cost));
        m_UpgradeButton->SetLabel("Upgrade");
    } else {
        char hpAtkBuf[128];
        snprintf(hpAtkBuf, sizeof(hpAtkBuf), "HP:  %.0f (MAX)\nATK: %.0f (MAX)", currentHp, currentAtk);
        m_StatsTextHpAtk->SetText(hpAtkBuf);
        m_StatsTextCost->SetText("XP COST:\nMAX");
        m_UpgradeButton->SetLabel("MAX LEVEL");
    }
}

void UpgradeScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_BackButton.reset();
    m_UpgradeButton.reset();
    m_XPText.reset();
    m_XPObject.reset();
    m_CatFoodText.reset();
    m_CatFoodObject.reset();
    m_CenterPreviewObject.reset();
    m_StatsTextName.reset();
    m_StatsObjName.reset();
    m_StatsTextHpAtk.reset();
    m_StatsObjHpAtk.reset();
    m_StatsTextCost.reset();
    m_StatsObjCost.reset();
    m_CatItems.clear();
    m_BackgroundImage.reset();
    m_BackgroundObject.reset();
}
