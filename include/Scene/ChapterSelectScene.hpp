#ifndef CHAPTER_SELECT_SCENE_HPP
#define CHAPTER_SELECT_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

#include <memory>
#include <vector>

class App;
class ImageTextButton;

class ChapterSelectScene : public IScene {
public:
    explicit ChapterSelectScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;

    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;

    std::shared_ptr<Util::Text> m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObject;

    std::shared_ptr<ImageTextButton> m_Chapter1Button;
    std::shared_ptr<ImageTextButton> m_Chapter2Button;
    std::shared_ptr<ImageTextButton> m_Chapter3Button;
    std::shared_ptr<ImageTextButton> m_BackButton;
};

#endif
