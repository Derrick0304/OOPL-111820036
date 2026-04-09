#ifndef ISCENE_HPP
#define ISCENE_HPP

#include "Util/GameObject.hpp"

#include <memory>

class IScene {
public:
    virtual ~IScene() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
    virtual std::shared_ptr<Util::GameObject> GetRoot() const = 0;
};

#endif
