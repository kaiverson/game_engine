#pragma once

#include "components/script_component.hpp"
#include "input_state.hpp"
#include "components/material_component.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

class ChangeColorScript : public ScriptComponent {
private:
    std::shared_ptr<MaterialComponent> material_component;

public: 
    ChangeColorScript() {}

    void start(GameObject &game_object) override {
        material_component = game_object.get_component<MaterialComponent>();
    }

    void update(GameObject &game_object) override {
        if (InputState::is_key_pressed(GLFW_KEY_PERIOD)) {
            material_component->material.smoothness += 0.5 * InputState::delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_COMMA)) {
            material_component->material.smoothness -= 0.5 * InputState::delta_time;
            
        }
    }
};