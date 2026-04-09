#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Scene/IScene.hpp"
#include "Util/Renderer.hpp"

#include <memory>

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_State; }

    void Start();
    void Update();
    void End();
    void ChangeScene(std::unique_ptr<IScene> nextScene);
    void RequestExit() { m_State = State::END; }

    Util::Renderer& GetRenderer() { return m_Renderer; }

private:
    void ApplyPendingSceneChange();

    State m_State = State::START;
    Util::Renderer m_Renderer;
    std::unique_ptr<IScene> m_CurrentScene;
    std::unique_ptr<IScene> m_PendingScene;
    bool m_IsUpdatingScene = false;
};

#endif
