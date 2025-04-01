#ifndef RENDER_MESH_COMPONENT_HPP
#define RENDER_MESH_COMPONENT_HPP

#include "component.hpp"
#include "mesh.hpp"
#include "material.hpp"

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <limits>
#include <cassert>

class RenderMeshComponent : public Component {
private: 
    std::shared_ptr<Mesh> mesh;
    std::vector<std::shared_ptr<Material>> materials;
    std::shared_ptr<TransformComponent> transform_component;
    glm::mat4 cached_transform;

public:
    explicit RenderMeshComponent(std::shared_ptr<Mesh> mesh) {
        set_mesh(mesh);
    }

    void set_mesh(std::shared_ptr<Mesh> mesh) {
        this->mesh = mesh;
        materials.resize(mesh ? mesh->get_submesh_count() : 0, nullptr);
    }
    
    bool set_material(size_t submesh_index, std::shared_ptr<Material> material) {        
        if (submesh_index >= materials.size()) {
            std::cerr << "submesh index out of range\n";
            return false;
        }
        
        materials[submesh_index] = material;
        return true;
    }

    bool render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& camera_position, const LightProperties& light_properties) {
        if (!mesh || !transform_component) return false;

        glm::mat4 current_transform = transform_component->get_transform();
            
        if (current_transform != cached_transform) {
            for (auto &material : materials) {
                if (material) material->set_uniform("transform", current_transform);
            }
            cached_transform = current_transform;
        }

        for (auto &material : materials) {
            material->set_uniform("projection", projection);
            material->set_uniform("view", view);

            material->set_uniform("light.ambient", light_properties.ambient);
            material->set_uniform("light.diffuse", light_properties.diffuse);
            material->set_uniform("light.direction", light_properties.direction);
        }

        bool success;
        mesh->upload_to_GPU();
        success = mesh->bind();
        if (!success) std::cout << "rmc: Mesh not bound!!!\n";
        for (size_t i = 0; i < mesh->get_submesh_count(); ++i) {
            if (i < materials.size() && materials[i]) {
                materials.at(i)->apply();
                success = mesh->draw_submesh(i);
                if (!success) std::cout << "Submesh " << i << " not drawn\n";
            }
        }
        return true;
    }

    std::vector<std::shared_ptr<Material>> get_materials() const { return materials; }

    void start(GameObject &game_object) override {
        transform_component = game_object.get_component<TransformComponent>();

        assert(transform_component && "GameObject lacks TransformComponent");
    }
};

#endif // RENDER_MESH_COMPONENT_HPP