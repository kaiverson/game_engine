#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "game_object.hpp"
#include "game_object_builder.hpp"
#include "render.hpp"

// class GameObjectBuilder;

class Scene {
private:
    std::vector<std::shared_ptr<GameObject>> game_objects;

public:
    Scene() = default;

    GameObjectBuilder create_game_object(const std::string &name) {
        auto game_object = std::make_shared<GameObject>(name);
        game_objects.push_back(game_object);
        return GameObjectBuilder(game_object, *this);
    }

    bool add_game_object(const std::shared_ptr<GameObject> &game_object) {
        if (std::find(game_objects.begin(), game_objects.end(), game_object) != game_objects.end()) {
            std::cerr << "Scene: attempted to add GameObject with duplicate name\n";
            return false;   
        }
        
        game_objects.push_back(game_object);
        return true;
    }

    void remove_game_object(const std::shared_ptr<GameObject> &game_object) {
        auto it = std::find(game_objects.begin(), game_objects.end(), game_object);
        if (it != game_objects.end()) {
            game_objects.erase(it);
        }
    }

    const std::vector<std::shared_ptr<GameObject>> &get_game_objects() const {
        return game_objects;
    }

    std::shared_ptr<GameObject> find_game_object(const std::string &name) const {
        auto it = std::find_if(game_objects.begin(), game_objects.end(), [&name](const auto &obj) { return obj->name == name; });
        return (it != game_objects.end()) ? *it : nullptr;
    }

    void start() {
        auto objects = game_objects;
        for (auto &obj : objects) {
            obj->start();
        }
    }

    void update() {
        auto objects = game_objects;
        for (auto &obj : objects) {
            obj->update();
        }
    }
};