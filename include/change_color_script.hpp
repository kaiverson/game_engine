#pragma once

#include "components/script_component.hpp"
#include "material.hpp"
#include "input_state.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

class ChangeColorScript : public ScriptComponent {
private:
    std::shared_ptr<Material> material;

public: 
    ChangeColorScript() {}

    void start(GameObject &game_object) override {
        auto render_mesh_component = game_object.get_component<RenderMeshComponent>();
        if (!render_mesh_component) {
            exit(1);
            return;
        }
        auto materials = render_mesh_component->get_materials();
        if (materials.size() == 0) {
            std::cout << "NO MATERIALS\n";
            exit(1);
            return;
        }
        material = materials[0];
    }

    void update(GameObject &game_object, const float delta_time) override {
        if (InputState::is_key_just_pressed(GLFW_KEY_8)) {
            material->set_blend_mode(BlendMode::Opaque);
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_9)) {
            material->set_blend_mode(BlendMode::AlphaBlend);
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_0)) {
            material->set_blend_mode(BlendMode::Additive);
        }

        if (InputState::is_key_just_pressed(GLFW_KEY_5)) {
            material->set_cull_mode(CullMode::None);
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_6)) {
            material->set_cull_mode(CullMode::Back);
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_7)) {
            material->set_cull_mode(CullMode::Front);
        }
    }
};