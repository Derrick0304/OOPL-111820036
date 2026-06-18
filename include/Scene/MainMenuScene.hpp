#ifndef MAIN_MENU_SCENE_HPP
#define MAIN_MENU_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UI/ImageTextButton.hpp"

#include <memory>

class App;

class MainMenuScene : public IScene {
public:
    explicit MainMenuScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;
    
    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;

    // 頂部與底部邊框
    std::shared_ptr<Util::Image> m_BorderImage;
    std::shared_ptr<Util::GameObject> m_TopBorder;
    std::shared_ptr<Util::GameObject> m_BottomBorder;

    // XP 顯示
    std::shared_ptr<Util::Image> m_XPIconImage;
    std::shared_ptr<Util::GameObject> m_XPIconObject;
    std::shared_ptr<Util::Text> m_XPText;
    std::shared_ptr<Util::GameObject> m_XPObject;

    // 貓罐頭顯示
    std::shared_ptr<Util::Image> m_CatFoodIconImage;
    std::shared_ptr<Util::GameObject> m_CatFoodIconObject;
    std::shared_ptr<Util::Text> m_CatFoodText;
    std::shared_ptr<Util::GameObject> m_CatFoodObject;

    // 按鈕
    std::shared_ptr<ImageTextButton> m_StartButton;
    std::shared_ptr<ImageTextButton> m_UpgradeButton;
    std::shared_ptr<ImageTextButton> m_EquipButton;
    std::shared_ptr<ImageTextButton> m_BackButton;
};

#endif
