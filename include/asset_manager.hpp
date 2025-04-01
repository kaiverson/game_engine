#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "material_builder.hpp"

#include <vector>
#include <memory>

class AssetManager {
private:
    std::vector<std::shared_ptr<Shader>> shaders;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;

public:
    AssetManager() = default;
    std::shared_ptr<Shader> create_shader();
    std::shared_ptr<Texture> create_texture();
    std::shared_ptr<Mesh> create_mesh();
    MaterialBuilder create_material();
};

#endif