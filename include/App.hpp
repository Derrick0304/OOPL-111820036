#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "UnitManager.hpp"
#include "Util/Renderer.hpp"
#include "Util/Image.hpp"
#include <memory>

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_State; }

    void Start();
    void Update();
    void End();

private:
    State m_State = State::START;
    Util::Renderer m_Renderer;
    
    std::shared_ptr<Util::GameObject> m_Root;
    std::unique_ptr<UnitManager> m_UnitManager;

    // 背景
    std::shared_ptr<Util::Image> m_BackgroundImage;
};

#endif
