#ifndef CHAPTER_SELECT_SCENE_HPP
#define CHAPTER_SELECT_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <vector>
#include <functional>

class App;

class ChapterSelectScene : public IScene {
public:
    explicit ChapterSelectScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void HandleInput();
    void UpdateCarousel(float dt);

    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;

    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;

    struct CarouselItem {
        std::shared_ptr<Util::Image> image;
        std::shared_ptr<Util::GameObject> object;
        std::function<void()> onClick;
    };
    std::vector<CarouselItem> m_CarouselItems;

    // Carousel 捲動變數
    float m_CurrentScrollX = 0.0f;
    float m_TargetScrollX = 0.0f;
    float m_CarouselY = 0.0f;
    float m_SpacingX = 400.0f;
    int m_CurrentIndex = 0;

    // 拖曳與點擊判斷
    bool m_IsDragging = false;
    float m_LastMouseX = 0.0f;
    float m_ClickStartX = 0.0f;
};

#endif
