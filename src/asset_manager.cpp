#include "asset_manager.hpp"

MaterialBuilder AssetManager::create_material() {
    auto material = std::make_shared<Material>();
    materials.push_back(material);
    return MaterialBuilder(material);
}