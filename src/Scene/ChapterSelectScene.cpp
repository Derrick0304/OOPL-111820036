#include "Scene/ChapterSelectScene.hpp"

#include "App.hpp"
#include "Scene/MainMenuScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>

using json = nlohmann::json;

ChapterSelectScene::ChapterSelectScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void ChapterSelectScene::Enter() {
    LOG_INFO("Entering ChapterSelectScene");

    m_CarouselY = 0.0f;
    m_SpacingX = 400.0f;

    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("ChapterSelectScene")) {
                auto& scene = layout["ChapterSelectScene"];
                if (scene.contains("Carousel")) {
                    m_CarouselY = scene["Carousel"]["y"].get<float>();
                    m_SpacingX = scene["Carousel"]["spacingX"].get<float>();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to parse UI_Layout.json in ChapterSelectScene: {}", e.what());
        }
    }

    // 載入新的背景圖片 (ChapterSelectSceneBackgrounds.png)
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/ChapterSelectSceneBackgrounds.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.34f, 1.34f};
    m_BackgroundObject->m_Transform.translation = {0.0f, 0.0f};
    m_Root->AddChild(m_BackgroundObject);

    // 定義 4 個選單項目 (返回、第一章、第二章、第三章)
    struct ItemInitData {
        std::string path;
        std::function<void()> onClick;
    };

    std::vector<ItemInitData> items = {
        { "/UI/Buttons/BackMenuButton.png", [this]() { m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App)); } },
        { "/UI/Buttons/ch1.png", [this]() { m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 1)); } },
        { "/UI/Buttons/ch2.png", [this]() { m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 2)); } },
        { "/UI/Buttons/ch3.png", [this]() { m_App.ChangeScene(std::make_unique<StageSelectScene>(m_App, 3)); } }
    };

    m_CarouselItems.clear();
    for (size_t i = 0; i < items.size(); ++i) {
        auto img = std::make_shared<Util::Image>(RESOURCE_DIR + items[i].path);
        auto obj = std::make_shared<Util::GameObject>(img, 10.0f);
        m_Root->AddChild(obj);
        m_CarouselItems.push_back({img, obj, items[i].onClick});
    }

    // 預設將 Chapter 1 (索引 1) 置中
    m_CurrentIndex = 1;
    m_CurrentScrollX = -m_CurrentIndex * m_SpacingX;
    m_TargetScrollX = m_CurrentScrollX;
}

void ChapterSelectScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    HandleInput();
    UpdateCarousel(dt);
}

void ChapterSelectScene::HandleInput() {
    glm::vec2 cursor = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_IsDragging = true;
        m_LastMouseX = cursor.x;
        m_ClickStartX = cursor.x;
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
            m_CurrentIndex = std::max(0, std::min((int)m_CarouselItems.size() - 1, m_CurrentIndex));
            m_TargetScrollX = -m_CurrentIndex * m_SpacingX;

            // 判斷是否為單純點擊（拖曳小於 10px）
            float dragDistance = std::abs(cursor.x - m_ClickStartX);
            if (dragDistance < 10.0f) {
                for (size_t i = 0; i < m_CarouselItems.size(); ++i) {
                    float x = m_CurrentScrollX + i * m_SpacingX;
                    float dist = std::abs(x);
                    float scale = 0.8f;
                    if (dist < m_SpacingX) {
                        scale = 0.8f + 0.3f * (1.0f - dist / m_SpacingX);
                    }
                    float cardWidth = 328.0f * scale;
                    float cardHeight = 263.0f * scale;
                    
                    if (cursor.x >= x - cardWidth / 2.0f && cursor.x <= x + cardWidth / 2.0f &&
                        cursor.y >= m_CarouselY - cardHeight / 2.0f && cursor.y <= m_CarouselY + cardHeight / 2.0f) {
                        LOG_INFO("Triggering index {}", i);
                        if (m_CarouselItems[i].onClick) {
                            m_CarouselItems[i].onClick();
                        }
                        break;
                    }
                }
            }
        }
    }
}

void ChapterSelectScene::UpdateCarousel(float dt) {
    if (!m_IsDragging) {
        m_CurrentScrollX += (m_TargetScrollX - m_CurrentScrollX) * 10.0f * dt;
    }

    for (size_t i = 0; i < m_CarouselItems.size(); ++i) {
        float x = m_CurrentScrollX + i * m_SpacingX;
        float dist = std::abs(x);
        float scale = 0.8f; // 未置中的卡片縮小至 0.8f
        
        // 置中卡片放大至 1.1f
        if (dist < m_SpacingX) {
            scale = 0.8f + 0.3f * (1.0f - dist / m_SpacingX);
        }

        m_CarouselItems[i].object->m_Transform.translation = {x, m_CarouselY};
        m_CarouselItems[i].object->m_Transform.scale = {scale, scale};
    }
}

void ChapterSelectScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_CarouselItems.clear();
    m_BackgroundImage.reset();
    m_BackgroundObject.reset();
}
