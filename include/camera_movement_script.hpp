#pragma once

#include "game_object.hpp"
#include "components/script_component.hpp"
#include "input_state.hpp"
#include "components/transform_component.hpp"
#include "components/camera_component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

class CameraMovementScript : public ScriptComponent {
public:
    float camera_speed = 10.0f;
    float mouse_sensitivity = 0.1f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float xpos_previous = 0.0f;
    float ypos_previous = 0.0f;
    std::shared_ptr<TransformComponent> transform;
    std::shared_ptr<CameraComponent> camera;

    void start(GameObject &game_object) override {
        transform = game_object.get_component<TransformComponent>();
        camera = game_object.get_component<CameraComponent>();

        xpos_previous = InputState::mouse_state.xpos;
        ypos_previous = InputState::mouse_state.ypos;
    }

    void update(GameObject &game_object) override {

        if (InputState::is_key_pressed(GLFW_KEY_UP)) {
            camera->field_of_view += 5 * InputState::delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_DOWN)) {
            camera->field_of_view -= 5 * InputState::delta_time;
        }

        if (InputState::is_key_just_pressed(GLFW_KEY_1)) {
            camera->clear_flags = CameraComponent::ClearFlags::Skybox;
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_2)) {
            camera->clear_flags = CameraComponent::ClearFlags::SolidColor;
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_3)) {
            camera->clear_flags = CameraComponent::ClearFlags::DepthOnly;
        }
        if (InputState::is_key_just_pressed(GLFW_KEY_4)) {
            camera->clear_flags = CameraComponent::ClearFlags::Nothing;
        }

        float delta_time = InputState::delta_time;

        // Handles keyboard input for movement
        if (InputState::is_key_pressed(GLFW_KEY_W)) {
            transform->position += glm::normalize(transform->get_front() * glm::vec3(1.0, 0.0, 1.0)) * camera_speed * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_S)) {
            transform->position -= glm::normalize(transform->get_front() * glm::vec3(1.0, 0.0, 1.0)) * camera_speed * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_A)) {
            transform->position -= glm::normalize(glm::cross(transform->get_front(), transform->get_up()) * glm::vec3(1.0, 0.0, 1.0)) * camera_speed * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_D)) {
            transform->position += glm::normalize(glm::cross(transform->get_front(), transform->get_up()) * glm::vec3(1.0, 0.0, 1.0)) * camera_speed * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_SPACE)) {
            transform->position += camera_speed * glm::vec3(0.0f, 1.0f, 0.0f) * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_LEFT_SHIFT)) {
            transform->position += camera_speed * glm::vec3(0.0f, -1.0f, 0.0f) * delta_time;
        }


        // If cursor is disconnected, don't look around
        if (InputState::cursor_disconnected) return;

        // Handles mouse input for looking around
        float xoffset = InputState::mouse_state.xpos - xpos_previous;
        float yoffset = ypos_previous - InputState::mouse_state.ypos;

        xpos_previous = InputState::mouse_state.xpos;
        ypos_previous = InputState::mouse_state.ypos;

        float offset = sqrtf(xoffset*xoffset + yoffset*yoffset);
        if (offset  > 150.0) {
            return;
        }


        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        transform->rotation = glm::quatLookAt(glm::normalize(front), glm::vec3(0.0f, 1.0f, 0.0f));
    }
};