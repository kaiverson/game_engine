#ifndef ENGINE_CONFIG_HPP
#define ENGINE_CONFIG_HPP

#include <glad/glad.h>

struct EngineConfig {
    GLuint screen_width = 1000;
    GLuint screen_height = 800;
    int target_fps = 120;
    bool wireframe_mode = false;
    bool debug_mode = false;
};

extern EngineConfig config;

#endif // ENGINE_CONFIG_HPP