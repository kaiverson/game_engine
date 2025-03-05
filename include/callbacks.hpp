#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "components/transform_component.hpp"
#include "components/camera_component.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int modes);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#endif // CALLBACKS_HPP