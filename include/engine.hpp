#pragma once
#include "common_headers.hpp"

class EngineCore {
public: 
    bool initialize();
    bool run();
    void shutdown();

private:
    GLFWwindow* window = nullptr;
    std::unique_ptr<Scene> active_scene;

    /* initialize */
    bool create_window();
    bool init_gl_context();
    bool setup_callbacks();
    bool setup_scene();

    /* run */
    void main_loop();
    void process_debug_input();
};