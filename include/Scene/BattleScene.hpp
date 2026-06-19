#ifndef BATTLE_SCENE_HPP
#define BATTLE_SCENE_HPP

#include "Battle/WaveSpawner.hpp"
#include "Battle/CatCannonEffect.hpp"
#include "Scene/IScene.hpp"
#include "Stage/StageData.hpp"
#include "UIManager.hpp"
#include "UnitManager.hpp"
#include "Tower.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "UI/ImageTextButton.hpp"

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
    std::shared_ptr<Util::GameObject> m_WorldRoot;

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

    // 鏡頭與捲動
    float m_CameraX = 0.0f;
    float m_TargetCameraX = 0.0f;
    bool m_IsDragging = false;
    float m_LastMouseX = 0.0f;

    // 升級數據
    const std::vector<float> m_WorkerUpgradeCosts = {100, 200, 500, 1000, 2000, 4000, 7000};
    const std::vector<float> m_MaxMoneyLevels = {1000, 1500, 2500, 4000, 6000, 9000, 13000, 20000};
    const std::vector<float> m_MoneyRateLevels = {150, 200, 300, 450, 700, 1000, 1500, 2200};

    std::shared_ptr<Util::Image> m_BackgroundImage;
    std::vector<std::shared_ptr<Util::GameObject>> m_Backgrounds;

    std::shared_ptr<Util::Text> m_StageTitleText;
    std::shared_ptr<Util::GameObject> m_StageTitleObject;
    std::shared_ptr<Util::GameObject> m_ResultObject; // 不再需要 m_ResultText
    std::shared_ptr<Util::GameObject> m_BlueBarObject;
    std::shared_ptr<Util::Text> m_XPRewardText;
    std::shared_ptr<Util::GameObject> m_XPRewardObject;
    std::shared_ptr<ImageTextButton> m_OkButton;

    // 暫停與設定介面
    bool m_IsPaused = false;
    std::shared_ptr<ImageTextButton> m_PauseButton;
    std::shared_ptr<Util::Image> m_SettingsImage;
    std::shared_ptr<Util::GameObject> m_SettingsObject;

    // 雙方基地與血量顯示
    std::shared_ptr<Tower> m_CatBase;
    std::shared_ptr<Tower> m_EnemyBase;
    std::shared_ptr<Util::Text> m_CatBaseHPText;
    std::shared_ptr<Util::GameObject> m_CatBaseHPObject;
    std::shared_ptr<Util::Text> m_EnemyBaseHPText;
    std::shared_ptr<Util::GameObject> m_EnemyBaseHPObject;

    bool m_DebugMode = false;
};

#endif
