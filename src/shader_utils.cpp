#include "shader_utils.hpp"

ShaderUniformLocations get_uniform_locations(GLuint shaderProgram) {
    ShaderUniformLocations locations;

    std::unordered_map<std::string, GLint*> uniform_map = {
        {"transform", &locations.transform_loc},
        {"projection", &locations.projection_loc},
        {"view", &locations.view_loc},
        {"viewPos", &locations.viewPos_loc},
        {"material.base_color", &locations.material_base_color_loc},
        {"material.base_map", &locations.material_base_map_loc},
        {"material.metallic_map", &locations.material_metallic_map_loc},
        {"material.smoothness", &locations.material_smoothness_loc},
        {"material.normal_map", &locations.material_normal_map_loc},   
        {"material.occlusion_map", &locations.material_occlusion_map_loc},
        {"light.direction", &locations.light_direction_loc},
        {"light.ambient", &locations.light_ambient_loc},
        {"light.diffuse", &locations.light_diffuse_loc},
        {"light.specular", &locations.light_specular_loc} 
    };

    for (const auto& pair : uniform_map) {
        *pair.second = glGetUniformLocation(shaderProgram, pair.first.c_str());
        if (*pair.second == -1) {
            std::cerr << "Could not find uniform location for: " << pair.first << std::endl;
        }
    }

    return locations;
}