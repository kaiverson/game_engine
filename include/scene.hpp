#pragma once

#include <vector>
#include <memory>
#include "game_object.hpp"

class Scene {
private:
    std::vector<std::shared_ptr<GameObject>> game_objects;

public:
    Scene() = default;

    std::shared_ptr<GameObject> create_game_object(const std::string &name) {
        std::shared_ptr<GameObject> game_object = std::make_shared<GameObject>(name);
        game_objects.push_back(game_object);
        return game_object;
    }

    void add_game_object(const std::shared_ptr<GameObject> &game_object) {
        game_objects.push_back(game_object);
    }

    void remove_game_object(const std::shared_ptr<GameObject> &game_object) {
        
    }

    void start() {
        for (auto &game_object : game_objects) {
            game_object->start();
        }
    }

    void update() {
        for (auto &game_object : game_objects) {
            game_object->update();
        }
    }
};