#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <imgui.h>

class GameObject; // Forward declaration

class Component {
public:
    virtual ~Component() = default;
    virtual void start(GameObject &game_object) {};
    virtual void update(GameObject &game_object, const float delta_time) {};

    virtual void draw_inspector_ui() {
        ImGui::Text("No UI available for this component.");
    };
};

#endif // COMPONENT_HPP