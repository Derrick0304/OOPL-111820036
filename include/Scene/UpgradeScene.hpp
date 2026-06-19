#ifndef UPGRADE_SCENE_HPP
#define UPGRADE_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UI/ImageTextButton.hpp"

#include <memory>
#include <vector>
#include <string>

class App;

class UpgradeScene : public IScene {
public:
    explicit UpgradeScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void HandleInput();
    void UpdateCarousel(float dt);
    void UpdateSelectedInfo();

    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;

    // 背景與邊界
    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;
    std::shared_ptr<Util::Image> m_BorderImage;
    std::shared_ptr<Util::GameObject> m_TopBorder;
    std::shared_ptr<Util::GameObject> m_BottomBorder;

    // 資源顯示
    std::shared_ptr<Util::Image> m_XPIconImage;
    std::shared_ptr<Util::GameObject> m_XPIconObject;
    std::shared_ptr<Util::Text> m_XPText;
    std::shared_ptr<Util::GameObject> m_XPObject;

    std::shared_ptr<Util::Image> m_CatFoodIconImage;
    std::shared_ptr<Util::GameObject> m_CatFoodIconObject;
    std::shared_ptr<Util::Text> m_CatFoodText;
    std::shared_ptr<Util::GameObject> m_CatFoodObject;

    // 按鈕
    std::shared_ptr<ImageTextButton> m_BackButton;
    std::shared_ptr<ImageTextButton> m_UpgradeButton;

    // 畫面中央預覽 (選中單位的立繪)
    std::shared_ptr<Util::GameObject> m_CenterPreviewObject;
    std::string m_LastPreviewedCatId;

    // 資訊面板文字
    std::shared_ptr<Util::Text> m_StatsTextName;
    std::shared_ptr<Util::GameObject> m_StatsObjName;
    std::shared_ptr<Util::Text> m_StatsTextHpAtk;
    std::shared_ptr<Util::GameObject> m_StatsObjHpAtk;
    std::shared_ptr<Util::Text> m_StatsTextCost;
    std::shared_ptr<Util::GameObject> m_StatsObjCost;

    // 可滑動的貓咪列表 (白色區域內)
    std::vector<std::string> m_AvailableCats;
    struct CatItem {
        std::string catId;
        std::shared_ptr<Util::Image> image;
        std::shared_ptr<Util::GameObject> object;
        std::shared_ptr<Util::Text> levelText;
        std::shared_ptr<Util::GameObject> levelObj;
    };
    std::vector<CatItem> m_CatItems;

    // 列表滾動與 snapping
    float m_CurrentScrollX = 0.0f;
    float m_TargetScrollX = 0.0f;
    float m_ListY = 37.0f;
    float m_SpacingX = 180.0f;
    float m_MaxScrollX = 0.0f;
    float m_MinScrollX = 0.0f;
    int m_SelectedIndex = 0;

    // 拖曳狀態
    bool m_IsDragging = false;
    float m_LastMouseX = 0.0f;
    bool m_MouseDownLastFrame = false;
    glm::vec2 m_MouseDownPos;
};

#endif
