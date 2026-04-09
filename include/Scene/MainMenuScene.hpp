#ifndef MAIN_MENU_SCENE_HPP
#define MAIN_MENU_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

#include <memory>

class App;
class TextButton;

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
    std::shared_ptr<Util::Image> m_PanelImage;
    std::shared_ptr<Util::GameObject> m_PanelObject;
    std::shared_ptr<Util::Text> m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObject;
    std::shared_ptr<TextButton> m_StartButton;
    std::shared_ptr<TextButton> m_ExitButton;
};

#endif
