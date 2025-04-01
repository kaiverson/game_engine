#pragma once

// #include "scene.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <iostream>

class Scene;

struct MouseState {
    float xpos = 0.0;
    float ypos = 0.0;
};

struct KeyState {
    std::unordered_map<int, bool> keys;
    std::unordered_map<int, bool> keys_previous;
};

struct MouseButtonState {
    std::unordered_map<int, bool> buttons;
    std::unordered_map<int, bool> buttons_previous;
};

class InputState {
public:
    static MouseState mouse_state;
    static KeyState key_state;
    static MouseButtonState mouse_button_state;
    static GLFWwindow *window;
    static Scene *active_scene;


    static void update_mouse_position(GLFWwindow* window, double xpos, double ypos) {
        mouse_state.xpos = static_cast<float>(xpos);
        mouse_state.ypos = static_cast<float>(ypos);
    }

    static void update_key_state(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (action == GLFW_PRESS) {
            key_state.keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            key_state.keys[key] = false;
        }
    }

    static void update_mouse_button_state(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS) {
            mouse_button_state.buttons[button] = true;
        } else if (action == GLFW_RELEASE) {
            mouse_button_state.buttons[button] = false;
        }
    }

    static bool is_key_pressed(int key) {
        return key_state.keys[key];
    }

    static bool is_key_just_pressed(int key) {
        return key_state.keys[key] && !key_state.keys_previous[key];
    }

    static bool is_mouse_button_pressed(int button) {
        return mouse_button_state.buttons[button];
    }

    static bool is_mouse_button_just_pressed(int button) {
        return mouse_button_state.buttons[button] && !mouse_button_state.buttons_previous[button];
    }

    static bool is_mouse_button_just_released(int button) {
        return !mouse_button_state.buttons[button] && mouse_button_state.buttons_previous[button];
    }

    static void update_previous_key_state() {
        // for (const auto &[key, pressed] : key_state.keys) {
        //     if (key_state.keys_previous[key] != pressed) {
        //         std::cout << "Key " << key << " state changed to " << pressed << "\n";
        //     }
        // }

        key_state.keys_previous = key_state.keys;
        mouse_button_state.buttons_previous = mouse_button_state.buttons;
    }

    static void set_window(GLFWwindow *window) {
        InputState::window = window;
    }

    static void set_active_scene(Scene *scene) {
        InputState::active_scene = scene;
    }
};

extern InputState input_state;