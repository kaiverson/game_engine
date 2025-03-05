#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <iostream>

struct MouseState {
    double xpos = 0.0;
    double ypos = 0.0;
};

struct KeyState {
    std::unordered_map<int, bool> keys;
    std::unordered_map<int, bool> keys_previous;
};

struct MouseButtonState {
    std::unordered_map<int, bool> buttons;
};

class InputState {
public:
    static MouseState mouse_state;
    static KeyState key_state;
    static MouseButtonState mouse_button_state;
    static double delta_time;

    static void update_mouse_position(GLFWwindow* window, double xpos, double ypos) {
        mouse_state.xpos = xpos;
        mouse_state.ypos = ypos;
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

    static void update_previous_key_state() {
        // for (const auto &[key, pressed] : key_state.keys) {
        //     if (key_state.keys_previous[key] != pressed) {
        //         std::cout << "Key " << key << " state changed to " << pressed << "\n";
        //     }
        // }

        key_state.keys_previous = key_state.keys;
    }
};

extern InputState input_state;