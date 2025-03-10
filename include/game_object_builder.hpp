#pragma once 
#include "game_object.hpp"
#include "components/transform_component.hpp"
#include "components/material_component.hpp"
#include "components/render_mesh_component.hpp"
#include "components/camera_component.hpp"
#include "components/script_component.hpp"

class Scene;

class GameObjectBuilder {
private:
    std::shared_ptr<GameObject> game_object;
    Scene &scene;
    std::shared_ptr<MaterialComponent> current_material;

public:
    GameObjectBuilder(const std::shared_ptr<GameObject>& game_object, Scene& scene)
    : game_object(game_object), scene(scene) {}

    GameObjectBuilder &with_transform(
        const glm::vec3 &position = glm::vec3(0.0f),
        const glm::quat &rotation = glm::quat(glm::vec3(0.0f)),
        const glm::vec3 &scale = glm::vec3(1.0f)) {

            auto transform = game_object->get_component<TransformComponent>();
            transform->position = position;
            transform->rotation = rotation;
            transform->scale = scale;
            return *this;
    }

    GameObjectBuilder &with_render_mesh(const std::string &model_path) {
        ObjModel model;
        if (load_obj(model_path, model)) {
            auto render_component = std::make_shared<RenderMeshComponent>(model);
            game_object->add_component(render_component);
        }
        return *this;
    }

    GameObjectBuilder &with_camera() {
        auto camera_component = std::make_shared<CameraComponent>();
        game_object->add_component(camera_component);
        return *this;
    }

    GameObjectBuilder &with_background(const glm::vec4 color) {
        auto camera_component = game_object->get_component<CameraComponent>();
        camera_component->background = color;
        camera_component->clear_flags = CameraComponent::ClearFlags::SolidColor;
        return *this;
    }

    GameObjectBuilder &with_skybox(const std::string &directory, GLuint shader) {
        auto camera_component = game_object->get_component<CameraComponent>();
        Skybox skybox = Skybox(directory, shader);
        camera_component->skybox = skybox;
        camera_component->clear_flags = CameraComponent::ClearFlags::Skybox;
        return *this;
    }

    GameObjectBuilder &with_material(std::shared_ptr<MaterialComponent> material = nullptr) {
        if (!material) {
            material = std::make_shared<MaterialComponent>();
        }
        game_object->add_component(material);
        current_material = material;
        return *this;
    }

    GameObjectBuilder &set_shader(GLuint shader) {
        if (current_material) {
            current_material->set_shader(shader);
        }
        return *this;
    }

    GameObjectBuilder &with_texture(
        const std::string &texture_path, 
        TextureType type = TextureType::Base
    ) {
        if (current_material) {
            current_material->set_texture(type, texture_path);
        }
        return *this;
    }

    GameObjectBuilder &with_script(const std::shared_ptr<ScriptComponent> &script) {
        game_object->add_component(script);
        return *this;
    }

    // TODO
    /*
    with_camera
    with_light

    Presets:
        as_environment_prop

    Serialization:
        from_json

    Async: 
        create_game_object_async
        with_model_async
        on_complete
    */

    std::shared_ptr<GameObject> build() {
        return game_object;
    }
};