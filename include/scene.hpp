#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "game_object.hpp"
#include "game_object_builder.hpp"
#include "components/camera_component.hpp"

class Scene {
private:
    std::vector<std::shared_ptr<GameObject>> game_objects_;
    std::shared_ptr<GameObject> main_camera_;
    std::shared_ptr<GameObject> main_light_;

public:
    Scene() = default;

    GameObjectBuilder create_game_object(const std::string &name) {
        auto game_object = std::make_shared<GameObject>(name);
        game_objects_.push_back(game_object);
        return GameObjectBuilder(game_object, *this);
    }

    bool add_game_object(const std::shared_ptr<GameObject> &game_object) {
        if (!game_object) return false;
        
        if (find_game_object(game_object->name)) {
            std::cerr << "Scene: object with name '" << game_object->name
                << "' already exists\n";
            return false;
        }
        
        game_objects_.push_back(game_object);
        return true;
    }

    void remove_game_object(const std::shared_ptr<GameObject> &game_object) {
        auto it = std::find(game_objects_.begin(), game_objects_.end(), game_object);
        if (it != game_objects_.end()) {
            game_objects_.erase(it);
        }
    }

    const std::vector<std::shared_ptr<GameObject>> &get_game_objects() const {
        return game_objects_;
    }

    std::shared_ptr<GameObject> find_game_object(const std::string &name) const {
        auto it = std::find_if(game_objects_.begin(), game_objects_.end(), [&name](const auto &obj) { return obj->name == name; });
        return (it != game_objects_.end()) ? *it : nullptr;
    }

    bool set_main_camera(std::shared_ptr<GameObject> camera_game_object) {
        if (!camera_game_object) {
            std::cerr << "Scene: null camera object provided\n";
            return false;
        }

        auto camera_component = camera_game_object->get_component<CameraComponent>();
        if (!camera_component) {
            std::cerr << "Scene: object '" << camera_game_object->name 
                << "' has no CameraComponent and can not be the main camera\n";
                return false;
        }

        main_camera_ = camera_game_object;
        return true;
    }

    std::shared_ptr<GameObject> get_main_camera() const {
        return main_camera_;
    }

    std::shared_ptr<CameraComponent> get_main_camera_component() const {
        if (!main_camera_) {
            std::cerr << "Scene: no main camera set\n";
            return nullptr;
        }

        auto camera_component = main_camera_->get_component<CameraComponent>();
        if (!camera_component) {
            std::cerr << "Scene: main camera missing CameraComponent\n";
        }
        
        return camera_component;
    }

    // bool set_light(std::shared_ptr<GameObject> light_game_object) {
    //     if (!light_game_object || !light_game_object->get_component<LightComponent>())
    // }



    void start() {
        for (auto &game_object : game_objects_) {
            game_object->start();
        }
    }

    void update(float delta_time) {
        for (auto &game_object : game_objects_) {
            game_object->update(delta_time);
        }
    }
};