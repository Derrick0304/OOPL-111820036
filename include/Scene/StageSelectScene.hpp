#ifndef STAGE_SELECT_SCENE_HPP
#define STAGE_SELECT_SCENE_HPP

#include "Scene/IScene.hpp"
#include "Stage/StageData.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

#include <memory>
#include <vector>

class App;
class TextButton;

class StageSelectScene : public IScene {
public:
    explicit StageSelectScene(App& app);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void BuildStageButtons();

    App& m_App;
    std::shared_ptr<Util::GameObject> m_Root;
    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::GameObject> m_BackgroundObject;
    std::shared_ptr<Util::Image> m_PanelImage;
    std::shared_ptr<Util::GameObject> m_PanelObject;
    std::shared_ptr<Util::Text> m_TitleText;
    std::shared_ptr<Util::GameObject> m_TitleObject;
    std::shared_ptr<Util::Text> m_MessageText;
    std::shared_ptr<Util::GameObject> m_MessageObject;
    std::vector<StageData> m_Stages;
    std::vector<std::shared_ptr<TextButton>> m_StageButtons;
    std::shared_ptr<TextButton> m_BackButton;
};

#endif
