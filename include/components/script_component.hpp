#pragma once

#include "component.hpp"
#include <string>
#include <functional>

class ScriptComponent : public Component {
public:
    ScriptComponent() = default;
    virtual ~ScriptComponent() = default;

    virtual void start(GameObject &game_object) override {}
    virtual void update(GameObject &game_object) override {}
};