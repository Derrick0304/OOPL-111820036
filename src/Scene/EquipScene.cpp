#include "Scene/EquipScene.hpp"

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

EquipScene::EquipScene(App& app)
    : m_App(app), m_Root(std::make_shared<Util::GameObject>()) {}

void EquipScene::Enter() {
    LOG_INFO("Entering EquipScene");
    m_App.PlayBGM(RESOURCE_DIR"/audio/MenuScene.mp3");

    float backX = -560.0f, backY = -290.0f;
    float xpX = 460.0f, xpY = 322.0f;
    int xpFontSize = 24;
    float catFoodX = 460.0f, catFoodY = -332.0f;
    int catFoodFontSize = 24;

    m_ListY = -140.0f;
    m_SpacingX = 130.0f;

    // 從 UI_Layout.json 讀取，若無則使用極簡預設坐標
    std::ifstream file(RESOURCE_DIR"/Data/UI_Layout.json");
    if (file.is_open()) {
        try {
            json layout = json::parse(file);
            if (layout.contains("EquipScene")) {
                auto& scene = layout["EquipScene"];
                if (scene.contains("BackButton")) {
                    backX = scene["BackButton"]["x"].get<float>();
                    backY = scene["BackButton"]["y"].get<float>();
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
            LOG_ERROR("Failed to parse UI_Layout.json in EquipScene: {}", e.what());
        }
    }

    // 1. 設置 10 個編隊格子中心坐標
    m_SlotCenters = {
        {-255.0f, 190.0f}, {-105.0f, 190.0f}, {45.0f, 190.0f}, {195.0f, 190.0f}, {345.0f, 190.0f},
        {-255.0f, 90.0f},  {-105.0f, 90.0f},  {45.0f, 90.0f},  {195.0f, 90.0f},  {345.0f, 90.0f}
    };

    // 2. 載入極簡主背景圖 (帶有木紋邊框與格子)
    m_BackgroundImage = std::make_shared<Util::Image>(RESOURCE_DIR"/Backgrounds/OrganizeCatsBg.png");
    m_BackgroundObject = std::make_shared<Util::GameObject>(m_BackgroundImage, -20.0f);
    m_BackgroundObject->m_Transform.scale = {1.34f, 1.34f}; // 配合 1280x720 解析度縮放
    m_BackgroundObject->m_Transform.translation = {0.0f, 0.0f};
    m_Root->AddChild(m_BackgroundObject);

    // 3. 建立返回按鈕
    m_BackButton = std::make_shared<ImageTextButton>(" ", [this]() {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
    }, "/UI/Buttons/Btn_Back_Circle.png");
    m_BackButton->SetFlashEnabled(false);
    m_BackButton->m_Transform.translation = {backX, backY};
    m_BackButton->SetZIndex(20.0f);
    m_Root->AddChild(m_BackButton);
    for (auto& part : m_BackButton->GetParts()) {
        m_Root->AddChild(part);
    }

    // 4. XP 顯示文字
    m_XPText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", xpFontSize, std::to_string(m_App.GetTotalXP()), Util::Color(255, 255, 255));
    m_XPObject = std::make_shared<Util::GameObject>(m_XPText, 19.0f);
    m_XPObject->m_Transform.translation = {xpX, xpY};
    m_Root->AddChild(m_XPObject);

    // 5. 貓罐頭顯示文字
    m_CatFoodText = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", catFoodFontSize, std::to_string(m_App.GetCatFood()), Util::Color(255, 255, 255));
    m_CatFoodObject = std::make_shared<Util::GameObject>(m_CatFoodText, 19.0f);
    m_CatFoodObject->m_Transform.translation = {catFoodX, catFoodY};
    m_Root->AddChild(m_CatFoodObject);

    // 6. 初始化 10 個已裝備格子的顯示 SlotItem
    m_SlotObjects.clear();
    for (int i = 0; i < 10; ++i) {
        auto obj = std::make_shared<Util::GameObject>(nullptr, 12.0f);
        obj->m_Transform.translation = m_SlotCenters[i];
        obj->m_Transform.scale = {0.98f, 0.98f}; // 調整頭像大小配合背景格子
        obj->SetVisible(false);
        m_Root->AddChild(obj);
        m_SlotObjects.push_back({nullptr, obj, ""});
    }

    // 7. 初始化下方已擁有貓咪的左右捲動列表
    m_AvailableCats = { "BasicCat", "TankCat", "AxeCat", "GrossCat", "CowCat", "BirdCat", "FishCat", "LizardCat", "TitanCat", "KillerCat" };
    m_CatListItems.clear();

    for (size_t i = 0; i < m_AvailableCats.size(); ++i) {
        std::string catId = m_AvailableCats[i];
        auto img = std::make_shared<Util::Image>(RESOURCE_DIR + UnitFactory::Get(catId).iconPath);
        auto obj = std::make_shared<Util::GameObject>(img, 10.0f);
        obj->m_Transform.scale = {0.98f, 0.98f}; // 調整列表頭像大小
        m_Root->AddChild(obj);

        int cost = UnitFactory::Get(catId).cost;
        auto costTxt = std::make_shared<Util::Text>(RESOURCE_DIR"/fonts/Inter.ttf", 16, "$" + std::to_string(cost), Util::Color(0, 0, 0));
        auto costObj = std::make_shared<Util::GameObject>(costTxt, 11.0f);
        m_Root->AddChild(costObj);

        m_CatListItems.push_back({catId, img, obj, costTxt, costObj});
    }

    m_DragSourceSlotIndex = -1;

    // 設置捲動邊界 (最左或最右被拖到中間時停止)
    m_MaxScrollX = 0.0f;
    m_MinScrollX = -static_cast<float>(m_AvailableCats.size() - 1) * m_SpacingX;
    m_CurrentScrollX = -300.0f; // 讓列表初始置中段展示
    m_TargetScrollX = m_CurrentScrollX;

    // 8. 建立拖曳跟隨的臨時頭像物件
    m_DragObject = std::make_shared<Util::GameObject>(nullptr, 25.0f); // 放置在最上層
    m_DragObject->m_Transform.scale = {0.98f, 0.98f};
    m_DragObject->SetVisible(false);
    m_Root->AddChild(m_DragObject);

    // 9. 更新當前已裝備的貓咪
    UpdateEquippedSlots();
}

void EquipScene::Update() {
    float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE)) {
        m_App.ChangeScene(std::make_unique<MainMenuScene>(m_App));
        return;
    }

    HandleInput();
    UpdateCarousel(dt);
    UpdateEquippedSlots();

    m_BackButton->Update();

    // 更新 XP 與 貓罐頭文字
    if (m_XPText) {
        m_XPText->SetText(std::to_string(m_App.GetTotalXP()));
    }
    if (m_CatFoodText) {
        m_CatFoodText->SetText(std::to_string(m_App.GetCatFood()));
    }
}

void EquipScene::HandleInput() {
    glm::vec2 cursor = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_MouseDownLastFrame = true;
        m_MouseDownPos = cursor;
        m_LastMouseX = cursor.x;
        m_DragSourceSlotIndex = -1; // 預設來源為滾動列表

        // 如果點擊了下方列表區域
        if (cursor.y >= -260.0f && cursor.y <= -20.0f) {
            // 尋找是否點擊在某個貓咪 Icon 上
            for (auto& item : m_CatListItems) {
                glm::vec2 itemPos = item.object->m_Transform.translation;
                float dx = std::abs(cursor.x - itemPos.x);
                float dy = std::abs(cursor.y - itemPos.y);

                if (dx <= 45.0f && dy <= 45.0f) {
                    m_DraggedCatId = item.catId;
                    m_DragImage = item.image;
                    m_DragStartPos = cursor;
                    break;
                }
            }
        } else {
            // 檢查是否點選了上方格子的貓咪
            int clickedSlot = GetSlotIndexAt(cursor);
            if (clickedSlot != -1 && !m_App.GetEquippedCats()[clickedSlot].empty()) {
                m_DraggedCatId = m_App.GetEquippedCats()[clickedSlot];
                m_DragImage = m_SlotObjects[clickedSlot].image;
                m_DragStartPos = cursor;
                m_DragSourceSlotIndex = clickedSlot;
            }
        }
    }

    if (m_MouseDownLastFrame) {
        if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
            // 判斷拖曳模式
            if (!m_DraggedCatId.empty()) {
                if (!m_IsDraggingCat && !m_IsDraggingList) {
                    if (m_DragSourceSlotIndex != -1) {
                        // 來自上方格子：拖曳超過 15 像素即進入拖曳狀態
                        float dist = glm::distance(cursor, m_DragStartPos);
                        if (dist > 15.0f) {
                            m_IsDraggingCat = true;
                            m_DragObject->SetDrawable(m_DragImage);
                            m_DragObject->SetVisible(true);
                        }
                    } else {
                        // 來自下方列表：維持原來的縱向拖曳觸發
                        float dy = cursor.y - m_DragStartPos.y;
                        float dx = std::abs(cursor.x - m_DragStartPos.x);
                        if (dy > 15.0f && dy > dx) {
                            m_IsDraggingCat = true;
                            m_DragObject->SetDrawable(m_DragImage);
                            m_DragObject->SetVisible(true);
                        } else if (std::abs(cursor.x - m_DragStartPos.x) > 10.0f) {
                            m_IsDraggingList = true;
                        }
                    }
                }
            } else {
                // 點空處直接為列表滾動
                if (!m_IsDraggingList && std::abs(cursor.x - m_MouseDownPos.x) > 10.0f) {
                    m_IsDraggingList = true;
                }
            }

            // 執行對應的拖曳邏輯
            if (m_IsDraggingCat) {
                m_DragObject->m_Transform.translation = cursor;
            } else if (m_IsDraggingList) {
                float dx = cursor.x - m_LastMouseX;
                m_CurrentScrollX += dx;
                m_CurrentScrollX = std::max(m_MinScrollX, std::min(m_MaxScrollX, m_CurrentScrollX));
                m_TargetScrollX = m_CurrentScrollX;
                m_LastMouseX = cursor.x;
            }
        } else {
            // 放開滑鼠按鍵
            m_MouseDownLastFrame = false;

            if (m_IsDraggingCat) {
                // 貓咪拖曳結束，進行碰撞偵測
                int slotIndex = GetSlotIndexAt(cursor);
                if (m_DragSourceSlotIndex != -1) {
                    // 來自上方格子
                    if (slotIndex != -1) {
                        if (slotIndex != m_DragSourceSlotIndex) {
                            // 拖到別的格子
                            // 唯一性替換：如果這隻貓咪已被編組在其他位置，則將該位置清空
                            for (int i = 0; i < 10; ++i) {
                                if (i != m_DragSourceSlotIndex && m_App.GetEquippedCats()[i] == m_DraggedCatId) {
                                    m_App.SetEquippedCat(i, "");
                                }
                            }
                            // 清空原來的 Slot，然後設定新 Slot
                            m_App.SetEquippedCat(m_DragSourceSlotIndex, "");
                            m_App.SetEquippedCat(slotIndex, m_DraggedCatId);
                            LOG_INFO("Moved cat from slot {} to slot {}", m_DragSourceSlotIndex + 1, slotIndex + 1);
                        }
                    } else {
                        // 拖到外面（取消這格的編組）
                        m_App.SetEquippedCat(m_DragSourceSlotIndex, "");
                        LOG_INFO("Removed cat from slot {} (dragged out)", m_DragSourceSlotIndex + 1);
                    }
                } else {
                    // 來自下方列表
                    if (slotIndex != -1) {
                        // 唯一性替換：如果這隻貓咪已被編組在其他位置，則將該位置清空
                        for (int i = 0; i < 10; ++i) {
                            if (m_App.GetEquippedCats()[i] == m_DraggedCatId) {
                                m_App.SetEquippedCat(i, "");
                            }
                        }
                        m_App.SetEquippedCat(slotIndex, m_DraggedCatId);
                        LOG_INFO("Equipped {} to slot {}", m_DraggedCatId, slotIndex + 1);
                    }
                }
                
                // 重置拖曳狀態
                m_IsDraggingCat = false;
                m_DraggedCatId = "";
                m_DragImage.reset();
                m_DragObject->SetVisible(false);
                m_DragSourceSlotIndex = -1;
            } else if (m_IsDraggingList) {
                m_IsDraggingList = false;
            } else {
                // 單純點擊判定（短位移）
                float clickDist = glm::distance(m_MouseDownPos, cursor);
                if (clickDist < 10.0f) {
                    // 如果點擊了上方的編組格子，就清空該格子 (移除編組)
                    int slotIndex = GetSlotIndexAt(cursor);
                    if (slotIndex != -1) {
                        m_App.SetEquippedCat(slotIndex, "");
                        LOG_INFO("Removed cat from slot {}", slotIndex + 1);
                    }
                }
            }
            
            m_DraggedCatId = "";
            m_DragImage.reset();
            m_DragSourceSlotIndex = -1;
        }
    }
}

void EquipScene::UpdateCarousel(float dt) {
    if (!m_IsDraggingList) {
        m_CurrentScrollX += (m_TargetScrollX - m_CurrentScrollX) * 10.0f * dt;
    }

    for (size_t i = 0; i < m_CatListItems.size(); ++i) {
        float x = m_CurrentScrollX + i * m_SpacingX;
        float dist = std::abs(x);
        float scale = 0.85f;

        if (dist < m_SpacingX) {
            scale = 0.85f + 0.27f * (1.0f - dist / m_SpacingX);
        }

        m_CatListItems[i].object->m_Transform.translation = {x, m_ListY};
        m_CatListItems[i].object->m_Transform.scale = {scale, scale};

        if (m_CatListItems[i].costObj) {
            // 文字位置置於 icon 下方與其重疊邊界處，縮放與 icon 同步
            m_CatListItems[i].costObj->m_Transform.translation = {x, m_ListY - 52.0f * scale};
            m_CatListItems[i].costObj->m_Transform.scale = {scale, scale};
        }
    }
}

void EquipScene::UpdateEquippedSlots() {
    auto equipped = m_App.GetEquippedCats();
    for (int i = 0; i < 10; ++i) {
        std::string catId = equipped[i];

        // 拔除動畫：如果這個 Slot 正在被拖出，就暫時隱藏它在格子上的顯示
        if (i == m_DragSourceSlotIndex && m_IsDraggingCat) {
            m_SlotObjects[i].object->SetVisible(false);
            continue;
        }

        if (m_SlotObjects[i].catId != catId) {
            m_SlotObjects[i].catId = catId;
            if (catId.empty()) {
                m_SlotObjects[i].object->SetVisible(false);
                m_SlotObjects[i].image.reset();
            } else {
                auto img = std::make_shared<Util::Image>(RESOURCE_DIR + UnitFactory::Get(catId).iconPath);
                m_SlotObjects[i].image = img;
                m_SlotObjects[i].object->SetDrawable(img);
                m_SlotObjects[i].object->SetVisible(true);
            }
        } else {
            // 如果 ID 沒有變，但之前可能在拖曳狀態而被隱藏，則需重新設為可見
            if (!catId.empty()) {
                m_SlotObjects[i].object->SetVisible(true);
            }
        }
    }
}

int EquipScene::GetSlotIndexAt(const glm::vec2& pos) const {
    for (int i = 0; i < 10; ++i) {
        glm::vec2 center = m_SlotCenters[i];
        float dx = std::abs(pos.x - center.x);
        float dy = std::abs(pos.y - center.y);
        // 格子放大後大小約 126x126，半徑 60 內算點中
        if (dx <= 60.0f && dy <= 60.0f) {
            return i;
        }
    }
    return -1;
}

void EquipScene::Exit() {
    m_Root = std::make_shared<Util::GameObject>();
    m_BackButton.reset();
    m_XPText.reset();
    m_XPObject.reset();
    m_CatFoodText.reset();
    m_CatFoodObject.reset();
    m_SlotObjects.clear();
    m_CatListItems.clear();
    m_DragObject.reset();
    m_BackgroundImage.reset();
    m_BackgroundObject.reset();
}
