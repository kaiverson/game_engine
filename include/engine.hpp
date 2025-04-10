#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "common_headers.hpp"

class EngineCore {
public: 
    bool initialize();
    bool run();
    void shutdown();

private:
    GLFWwindow* window = nullptr;
    std::unique_ptr<Scene> active_scene;
    GameObject* selected_game_object = nullptr;

    /* initialize */
    bool create_window();
    bool init_gl_context();
    bool setup_callbacks();
    bool setup_scene();

    /* run */
    void main_loop();
    void process_debug_input();
    bool render_scene(const float aspect_ratio);
    bool render_ui();

    void draw_properties_window();
};

#endif /* ENINE_HPP */