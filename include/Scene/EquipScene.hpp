#ifndef EQUIP_SCENE_HPP
#define EQUIP_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UI/ImageTextButton.hpp"

#include <memory>
#include <vector>
#include <string>

class App;

class EquipScene : public IScene {
public:
    explicit EquipScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void HandleInput();
    void UpdateCarousel(float dt);
    void UpdateEquippedSlots();
    int GetSlotIndexAt(const glm::vec2& pos) const;

    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;

    // 背景
    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;

    // 返回按鈕
    std::shared_ptr<ImageTextButton> m_BackButton;

    // XP 與 貓罐頭文字顯示
    std::shared_ptr<Util::Text> m_XPText;
    std::shared_ptr<Util::GameObject> m_XPObject;
    std::shared_ptr<Util::Text> m_CatFoodText;
    std::shared_ptr<Util::GameObject> m_CatFoodObject;

    // 10 個已裝備編隊格子顯示物件
    struct SlotItem {
        std::shared_ptr<Util::Image> image;
        std::shared_ptr<Util::GameObject> object;
        std::string catId;
    };
    std::vector<SlotItem> m_SlotObjects;

    // 下方已擁有的貓咪列表 (左右捲動)
    std::vector<std::string> m_AvailableCats;
    struct CatListItem {
        std::string catId;
        std::shared_ptr<Util::Image> image;
        std::shared_ptr<Util::GameObject> object;
        std::shared_ptr<Util::Text> costText;
        std::shared_ptr<Util::GameObject> costObj;
    };
    std::vector<CatListItem> m_CatListItems;

    // 10 個 Slot 在 PTSD 中的座標
    std::vector<glm::vec2> m_SlotCenters;

    // 捲動列表控制變數
    float m_CurrentScrollX = 0.0f;
    float m_TargetScrollX = 0.0f;
    float m_ListY = -140.0f;
    float m_SpacingX = 130.0f;
    float m_MaxScrollX = 0.0f;
    float m_MinScrollX = 0.0f;

    // 拖曳狀態變數
    bool m_IsDraggingList = false;
    bool m_IsDraggingCat = false;
    float m_LastMouseX = 0.0f;
    glm::vec2 m_DragStartPos;
    std::string m_DraggedCatId;
    std::shared_ptr<Util::Image> m_DragImage;
    std::shared_ptr<Util::GameObject> m_DragObject;
    int m_DragSourceSlotIndex = -1;

    // 點擊事件判斷
    bool m_MouseDownLastFrame = false;
    glm::vec2 m_MouseDownPos;
};

#endif
