#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <typeinfo>

#include "structs.hpp"
#include "components/component.hpp"
#include "components/transform_component.hpp"

class GameObject {
    public:
    std::string name;
    std::vector<std::shared_ptr<Component>> components;

    GameObject(std::string name) : name(name) {
        auto transform_component = std::make_shared<TransformComponent>();
        components.push_back(transform_component);
    }

    bool add_component(std::shared_ptr<Component> component) {
        if (std::dynamic_pointer_cast<TransformComponent>(component)) {
            std::cerr << "Error: Cannot add a TransformComponent to GameObject " << name <<". It already has one by default.\n";
            return false;
        }

        components.push_back(component);
        return true;
    }

    template <typename T>
    std::shared_ptr<T> get_component() {
        for (auto& component : components) {
            if (auto casted = std::dynamic_pointer_cast<T>(component)) {
                return casted;
            }
        }

        return nullptr;
    }

    void start() {
        for (auto &component : components) {
            component->start(*this);
        }
    }

    void update() {
        for (auto &component : components) {
            component->update(*this);
        }
    }
};