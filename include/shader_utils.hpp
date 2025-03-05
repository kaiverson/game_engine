#ifndef SHADER_UTILS_HPP
#define SHADER_UTILS_HPP

#include <glad/glad.h>
#include <unordered_map>
#include <string>
#include <iostream>

struct ShaderUniformLocations {
    GLint transform_loc;
    GLint projection_loc;
    GLint view_loc;
    GLint viewPos_loc;
    GLint material_base_color_loc;
    GLint material_base_map_loc;
    GLint material_metallic_map_loc;
    GLint material_smoothness_loc;
    GLint material_normal_map_loc;
    GLint material_height_map_loc;
    GLint material_occlusion_map_loc;
    GLint light_direction_loc;
    GLint light_ambient_loc;
    GLint light_diffuse_loc;
    GLint light_specular_loc;
};

ShaderUniformLocations get_uniform_locations(GLuint shaderProgram);

#endif // SHADER_UTILS_HPP