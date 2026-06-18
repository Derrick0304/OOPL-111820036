#ifndef START_SCENE_HPP
#define START_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/Image.hpp"
#include "UI/ImageTextButton.hpp"
#include <memory>

class App;

class StartScene : public IScene {
public:
    explicit StartScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;

    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;

    std::shared_ptr<ImageTextButton> m_PlayButton;
};

#endif
