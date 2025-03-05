#ifndef MATERIAL_COMPONENT_HPP
#define MATERIAL_COMPONENT_HPP

#include "component.hpp"
#include "texture_loader.hpp"
#include <glm/glm.hpp>

struct MaterialProperties {
    glm::vec3 base_color;
    GLuint base_map;
    GLuint metallic_map;
    float smoothness;
    GLuint normal_map;
    GLuint occlusion_map;

    MaterialProperties()
        : base_color(glm::vec3(1.0f)), base_map(0), metallic_map(0), smoothness(0.5f), normal_map(0), occlusion_map(0) {}
};

class MaterialComponent : public Component {
public:
    MaterialProperties material;

    MaterialComponent(const MaterialProperties &material = {}) 
        : material(material) {}

    void set_texture(GLuint &texture, const std::string &path) {
        // Load texture from file and set it to the given texture ID
        texture = load_texture(path.c_str());
    }

    void update(GameObject& game_object) override {
        // Material properties don't need to update the game object
    }
};

#endif // MATERIAL_COMPONENT_HPP