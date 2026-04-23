#ifndef BATTLE_SCENE_HPP
#define BATTLE_SCENE_HPP

#include "Battle/WaveSpawner.hpp"
#include "Battle/CatCannonEffect.hpp"
#include "Scene/IScene.hpp"
#include "Stage/StageData.hpp"
#include "UIManager.hpp"
#include "UnitManager.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

#include <memory>

class App;
class TextButton;

class BattleScene : public IScene {
public:
    BattleScene(App& app, StageData stage);

    void Enter() override;
    void Update() override;
    void Exit() override;
    std::shared_ptr<Util::GameObject> GetRoot() const override { return m_Root; }

private:
    void SetupBattlefield();
    void SetupResultOverlay();
    void ShowResult(const std::string& resultText);

    App& m_App;
    StageData m_Stage;
    std::shared_ptr<Util::GameObject> m_Root;
    std::unique_ptr<UnitManager> m_UnitManager;
    std::unique_ptr<UIManager> m_UIManager;
    std::unique_ptr<WaveSpawner> m_WaveSpawner;
    std::unique_ptr<CatCannonEffect> m_CatCannonEffect;

    float m_Money = 100.0f;
    float m_MaxMoney = 1000.0f;
    float m_MoneyPerSecond = 150.0f;
    int m_WorkerLevel = 1;

    float m_CannonCooldown = 0.0f;
    const float CANNON_MAX_COOLDOWN = 30.0f; // 30 秒冷卻
    
    float m_CannonEffectTimer = -1.0f;
    std::set<Unit*> m_CannonHitList;

    bool m_BattleEnded = false;

    // 升級數據
    const std::vector<float> m_WorkerUpgradeCosts = {100, 200, 500, 1000, 2000, 4000, 7000};
    const std::vector<float> m_MaxMoneyLevels = {1000, 1500, 2500, 4000, 6000, 9000, 13000, 20000};
    const std::vector<float> m_MoneyRateLevels = {150, 200, 300, 450, 700, 1000, 1500, 2200};

    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::shared_ptr<Util::Text> m_StageTitleText;
    std::shared_ptr<Util::GameObject> m_StageTitleObject;
    std::shared_ptr<Util::Text> m_ResultText;
    std::shared_ptr<Util::GameObject> m_ResultObject;
    std::shared_ptr<TextButton> m_RetryButton;
    std::shared_ptr<TextButton> m_StageSelectButton;
    std::shared_ptr<TextButton> m_MainMenuButton;
};

#endif
