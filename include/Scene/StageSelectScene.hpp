#ifndef STAGE_SELECT_SCENE_HPP
#define STAGE_SELECT_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Stage/StageData.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UI/ImageTextButton.hpp"

#include <memory>
#include <vector>

class App;

class StageSelectScene : public IScene {
public:
    explicit StageSelectScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void UpdateCarousel(float dt);
    void HandleInput();

    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;
    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;
    
    std::shared_ptr<Util::Text> m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObject;

    std::vector<StageData> m_Stages;
    
    // 捲動列表項目 (關卡卡片)
    struct StageItem {
        std::shared_ptr<Util::GameObject> base;
        std::shared_ptr<Util::GameObject> textObj;
        StageData data;
    };
    std::vector<StageItem> m_StageItems;

    std::shared_ptr<ImageTextButton> m_BackButton;
    std::shared_ptr<ImageTextButton> m_StartButton;

    // 捲動邏輯變數
    float m_CurrentScrollX = 0.0f;
    float m_TargetScrollX = 0.0f;
    float m_CarouselY = 0.0f;
    float m_SpacingX = 350.0f;
    int m_CurrentIndex = 0;

    // 拖拽變數
    bool m_IsDragging = false;
    float m_LastMouseX = 0.0f;
};

#endif
