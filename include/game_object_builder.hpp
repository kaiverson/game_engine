#pragma once 
#include "game_object.hpp"
#include "material.hpp"
#include "components/transform_component.hpp"
#include "components/render_mesh_component.hpp"
#include "components/camera_component.hpp"
#include "components/script_component.hpp"

class Scene;

class GameObjectBuilder {
private:
    std::shared_ptr<GameObject> game_object;
    Scene &scene;
    std::shared_ptr<Material> current_material;

public:
    GameObjectBuilder(const std::shared_ptr<GameObject>& game_object, Scene& scene)
    : game_object(game_object), scene(scene) {}

    GameObjectBuilder &with_transform(const TransformParams &params) {

            auto transform = game_object->get_component<TransformComponent>();
            transform->position = params.position;
            transform->rotation = params.rotation;
            transform->scale = params.scale;
            return *this;
    }

    GameObjectBuilder &with_render_mesh(const std::string &model_path) {
        auto mesh = std::make_shared<Mesh>();
        if (!mesh->load(model_path)) {
            std::cerr << "GameObject " << game_object->name << ": failed to load mesh at " << model_path << "\n";
            return *this;
        }

        auto render_component = std::make_shared<RenderMeshComponent>(mesh);
        game_object->add_component(render_component);
        
        return *this;
    }

    GameObjectBuilder &with_camera() {
        auto camera_component = std::make_shared<CameraComponent>();
        game_object->add_component(camera_component);
        return *this;
    }

    GameObjectBuilder &with_background(const glm::vec4 color) {
        auto camera_component = game_object->get_component<CameraComponent>();
        if (!camera_component) {
            std::cerr << "Warning: attempted to change background on " << game_object->name << " which doesn't have a camera component.\n";
            return *this;
        }

        camera_component->background = color;
        camera_component->clear_flags = CameraComponent::ClearFlags::SolidColor;
        return *this;
    }

    // GameObjectBuilder &with_skybox(const std::string &directory) {
    //     auto camera_component = game_object->get_component<CameraComponent>();
    //     Skybox skybox = Skybox(directory, shader);
    //     camera_component->skybox = skybox;
    //     camera_component->clear_flags = CameraComponent::ClearFlags::Skybox;
    //     return *this;
    // }

    GameObjectBuilder &with_material(std::shared_ptr<Material> material, GLuint submesh_index = 0) {
        auto render_mesh_component = game_object->get_component<RenderMeshComponent>();
        if (!render_mesh_component) {
            std::cerr << "GameObject " << game_object->name << ": can't add a material before it has a render mesh component\n";
            return *this;
        }

        bool success = render_mesh_component->set_material(submesh_index, material);
        if (!success) {
            std::cout << "GameObject " << game_object->name << ": unable to add material at submesh_index " << submesh_index << "\n";
            return *this;
        }

        current_material = material;
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