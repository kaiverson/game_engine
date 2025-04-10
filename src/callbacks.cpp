#include "callbacks.hpp"
#include "input_state.hpp"
#include "game_object.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

extern std::shared_ptr<GameObject> camera_object;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    InputState::update_mouse_position(window, xpos, ypos);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int modes) {
    InputState::update_key_state(window, key, scancode, action, modes);
    
    extern bool wireframe_mode;

    // WIREFRAME MODE
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        wireframe_mode = !wireframe_mode;
        if (wireframe_mode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    // FREE CURSOR
    // if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !InputState::cursor_disconnected) {
    //     InputState::cursor_disconnected = true;
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // }

    // if (key == GLFW_KEY_F3) {
    //     if (action == GLFW_PRESS) {
    //         debug_mode = !debug_mode;
    //     } 
    // }

    // if (key == GLFW_KEY_N) {
    //     if (action == GLFW_PRESS) {
    //         normals_mode = !normals_mode;
    //     }
    // }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputState::update_mouse_button_state(window, button, action, mods);
    
    // if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && InputState::cursor_disconnected) {
    //     InputState::cursor_disconnected = false;
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // }
}

void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << "\n";
}