#pragma once

#include "game_object.hpp"
#include "components/script_component.hpp"
#include "input_state.hpp"
#include "components/transform_component.hpp"
#include "components/camera_component.hpp"
// #include "raycasting.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

extern GLuint SCREEN_WIDTH;
extern GLuint SCREEN_HEIGHT;

class CameraMovementScript : public ScriptComponent {
public:
    float camera_speed = 10.0f;
    float mouse_sensitivity = 0.1f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float xpos_previous = 0.0f;
    float ypos_previous = 0.0f;
    bool view_mode = false;

    glm::vec3 velocity = glm::vec3(0.0f);
    float acceleration_rate = camera_speed * 10;
    float deceleration_rate = camera_speed * 4;
    GLFWwindow *window;
    Scene *scene;
    std::shared_ptr<TransformComponent> transform;
    std::shared_ptr<CameraComponent> camera;

    void start(GameObject &game_object) override {
        transform = game_object.get_component<TransformComponent>();
        camera = game_object.get_component<CameraComponent>();

        xpos_previous = InputState::mouse_state.xpos;
        ypos_previous = InputState::mouse_state.ypos;

        window = InputState::window;
        scene = InputState::active_scene;
    }

    void update(GameObject &game_object, const float delta_time) override {
        if (InputState::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT) && view_mode == false) {
            view_mode = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            xpos_previous = InputState::mouse_state.xpos;
            ypos_previous = InputState::mouse_state.ypos;
        } else if (!InputState::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT) && view_mode == true) {
            view_mode = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // if (view_mode == false && InputState::is_mouse_button_just_released(GLFW_MOUSE_BUTTON_LEFT)) {
        //     int mouse_x = static_cast<int>(InputState::mouse_state.xpos);
        //     int mouse_y = static_cast<int>(InputState::mouse_state.ypos);
        //     Ray ray = screen_to_world_ray(mouse_x, mouse_y, SCREEN_WIDTH, SCREEN_HEIGHT, camera->get_view_matrix(transform->position, transform->get_front(), transform->get_up()), camera->get_projection_matrix(static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT));
        //     auto selected_object = raycast(ray, scene->get_game_objects());

        //     if (selected_object) {
        //         std::cout << selected_object->name << "\n";
        //     } else {
        //         std::cout << "Did not hit object\n";
        //     }
        // }


        if (InputState::is_key_just_pressed(GLFW_KEY_F)) {
            // get camera position
            glm::vec3 camera_position = transform->position;
            
            // get selected object (as determined by inspector)
            std::shared_ptr<GameObject> selected_game_object;

            // glm::vec3 selected_game_object_position = selected_game_object->get_component<TransformComponent>()->position;
            glm::vec3 selected_game_object_position = glm::vec3(0.0f);

            glm::vec3 look_direction = camera_position - selected_game_object_position;

            glm::normalize(look_direction);

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            transform->rotation = glm::quatLookAt(glm::normalize(front), glm::vec3(0.0f, 1.0f, 0.0f));
            // compute camera position - selected object position
            // normalize that
            // point the camera in that direction.
        }

        if (InputState::is_key_pressed(GLFW_KEY_UP)) {
            camera->field_of_view += 5 * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_DOWN)) {
            camera->field_of_view -= 5 * delta_time;
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

        glm::vec3 direction = glm::vec3(0.0f);

        // Determine movement direction
        if (InputState::is_key_pressed(GLFW_KEY_W)) {
            direction += glm::normalize(transform->get_front() * glm::vec3(1.0, 0.0, 1.0));
        }
        if (InputState::is_key_pressed(GLFW_KEY_S)) {
            direction -= glm::normalize(transform->get_front() * glm::vec3(1.0, 0.0, 1.0)) * camera_speed * delta_time;
        }
        if (InputState::is_key_pressed(GLFW_KEY_A)) {
            direction -= glm::normalize(glm::cross(transform->get_front(), transform->get_up()) * glm::vec3(1.0, 0.0, 1.0));
        }
        if (InputState::is_key_pressed(GLFW_KEY_D)) {
            direction += glm::normalize(glm::cross(transform->get_front(), transform->get_up()) * glm::vec3(1.0, 0.0, 1.0));
        }
        if (InputState::is_key_pressed(GLFW_KEY_SPACE)) {
            direction += glm::vec3(0.0f, 1.0f, 0.0f);
        }
        if (InputState::is_key_pressed(GLFW_KEY_LEFT_SHIFT)) {
            direction+= glm::vec3(0.0f, -1.0f, 0.0f);
        }

        // Apply acceleration or deceleration
        if (glm::length(direction) > 0.0f) {
            direction = glm::normalize(direction);

            velocity += direction * acceleration_rate * delta_time;
            if (glm::length(velocity) > camera_speed) {
                velocity = glm::normalize(velocity) * camera_speed;
            }
        } else {
            if (glm::length(velocity) > 0.0f) {
                glm::vec3 deceleration_vector = glm::normalize(velocity) * deceleration_rate * delta_time;
                if (glm::length(deceleration_vector) > glm::length(velocity)) {
                    velocity = glm::vec3(0.0f);
                } else {
                    velocity -= deceleration_vector;
                }
            }
        }

        transform->position += velocity * delta_time;


        // Don't look around if not in view_mode
        if (!view_mode) {
            return;
        }

        // Handles mouse input for looking around
        float xoffset = InputState::mouse_state.xpos - xpos_previous;
        float yoffset = ypos_previous - InputState::mouse_state.ypos;

        xpos_previous = (float)InputState::mouse_state.xpos;
        ypos_previous = (float)InputState::mouse_state.ypos;

        // float offset = sqrtf(xoffset*xoffset + yoffset*yoffset);
        // if (offset  > 150.0) {
        //     return;
        // }


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