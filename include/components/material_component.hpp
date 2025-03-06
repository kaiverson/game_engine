#ifndef MATERIAL_COMPONENT_HPP
#define MATERIAL_COMPONENT_HPP

#include "component.hpp"
#include "texture_loader.hpp"
#include <glm/glm.hpp>
#include <iostream>

enum class TextureType {
    Base,
    Normal,
    Metallic,
    Roughness,
    Occlusion
};

struct MaterialProperties {
    glm::vec3 base_color = glm::vec3(1.0f);
    GLuint base_map = 0;
    float smoothness = 0.5f;
    GLuint normal_map = 0;
    GLuint metallic_map = 0;
    GLuint occlusion_map = 0;
};

// TODO: make a few pre made shaders so I don't have to specify as much to make a game object

class MaterialComponent : public Component {
public:
    MaterialProperties material;

    MaterialComponent() : shader_program(0) {}
    explicit MaterialComponent(const MaterialProperties &material) 
        : material(material), shader_program(0) {}

    bool set_texture(TextureType type, const std::string &path) {
        GLuint texture_id = load_texture(path.c_str());
        if (texture_id == 0) {
            std::cerr << "Failed to load texture: " << path << "\n";
            return false;
        }
        
        switch (type) {
            case TextureType::Base:
                material.base_map = texture_id;
                break;
            case TextureType::Normal:
                material.normal_map = texture_id;
                break;
            case TextureType::Metallic:
                material.metallic_map = texture_id;
                break;
            case TextureType::Occlusion:
                material.occlusion_map = texture_id;
                break;
            default:
                std::cerr << "Unsupported texture type\n";
                return false;
        }
        return true;
    }

    void set_shader(GLuint shader_program) {
        if (glIsProgram(shader_program)) {
            this->shader_program = shader_program;
        } else {
            std::cerr << "Invalid shader program\n";
        }
    }

    GLuint get_shader() const { return shader_program; }

    void update(GameObject& game_object) override {
        // Optional
    }

private:
    GLuint shader_program;
};

#endif // MATERIAL_COMPONENT_HPP