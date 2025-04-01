#include "input_state.hpp"

MouseState InputState::mouse_state;
KeyState InputState::key_state;
MouseButtonState InputState::mouse_button_state;
GLFWwindow *InputState::window = nullptr;
Scene *InputState::active_scene = nullptr;