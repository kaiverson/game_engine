#pragma once

#include "material.hpp"

#include <memory>

enum MaterialPreset {
    Simple,
    URP,
    // Skybox
};

class MaterialBuilder {
private:
    std::shared_ptr<Material> material = nullptr;
    std::shared_ptr<Shader> shader = nullptr;

public:
    explicit MaterialBuilder(const std::shared_ptr<Material> material) : material(material) {}

    MaterialBuilder &with_preset(MaterialPreset preset = MaterialPreset::Simple) {
        material = std::make_shared<Material>();
        auto base_map_texture = std::make_shared<Texture>("src/objects/dragon/Material_baseColor.png");
        auto metallic_map_texture = std::make_shared<Texture>("src/objects/dragon/Material_normal.png");
        auto normal_map_texture = std::make_shared<Texture>("src/objects/dragon/Material_normal.png");
        float smoothness = 5.0;
        
        switch (preset) {
            case MaterialPreset::Simple:
                shader = std::make_shared<Shader>("src/shaders/simple");
                material->set_shader(shader);
                material->set_uniform("material.base_color", glm::vec3(1.0, 1.0, 1.0));
                break;
            case MaterialPreset::URP:
                shader = std::make_shared<Shader>("src/shaders/urp");
                material->set_shader(shader);
                material->set_uniform("material.base_map", base_map_texture);
                // material->set_uniform("material.metallic_map", metallic_map_texture);
                material->set_uniform("material.base_color", glm::vec3(1.0, 0, 1.0));
                // material->set_uniform("material.normal_map", normal_map_texture);
                material->set_uniform("material.smoothness", smoothness);
                break;
            // case MaterialPreset::Skybox:
            //     shader = std::make_shared<Shader>("src/shaders/skybox");
            //     material->set_shader(shader);
            //     material->set_depth_test(false);
            //     material->set_cull_mode(CullMode::Front);
            //     break;
            default:
                std::cerr << "MaterialBuilder: invalid preset\n";
                break;
        }

        return *this;
    }

    MaterialBuilder &with_uniform(const std::string &name, const std::shared_ptr<Texture> &texture) {
        bool success;
        success = material->set_uniform(name, texture);
        if (!success) {
            std::cerr << "MaterialBuilder: unable to set " << name << " uniform\n";
            return *this;
        }

        return *this;
    }

    // MaterialBuilder &with_metallic_roughness(float metallic, float roughness) {
    //     material->set_uniform("u_Metallic", metallic);
    //     material->set_uniform("u_Roughness", roughness);
    //     return *this;
    // }

    // MaterialBuilder &with_blend_mode(BlendMode mode) {
    //     material->set_blend_mode(mode);
    //     return *this;
    // }

    std::shared_ptr<Material> build() {
        return material;
    }
};