#include "object_loader.hpp"  
#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <cmath>  

bool load_obj(const std::string &filename, ObjModel &model) {  
    std::ifstream file(filename);  
    if (!file.is_open()) {  
        std::cerr << "Failed to open file: " << filename << std::endl;  
        return false;  
    }  

    auto resolve_index = [](int idx, size_t size) -> int {  
        if (idx == 0) return -1;  // OBJ indices are 1-based, 0 is invalid  
        return (idx < 0) ? static_cast<int>(size) + idx : idx - 1;  
    };  

    std::string line;  
    while (std::getline(file, line)) {  
        std::istringstream iss(line);  
        std::string prefix;  
        iss >> prefix;  

        if (prefix == "v") {  
            glm::vec3 vertex;  
            iss >> vertex.x >> vertex.y >> vertex.z;  
            model.vertices.push_back(vertex);  
        } else if (prefix == "vt") {  
            glm::vec2 tex_coord;  
            iss >> tex_coord.x >> tex_coord.y;  
            tex_coord.y = 1.0f - tex_coord.y;  // Flip UV Y-axis for OpenGL  
            model.texture_coords.push_back(tex_coord);  
        } else if (prefix == "vn") {  
            glm::vec3 normal;  
            iss >> normal.x >> normal.y >> normal.z;  
            model.normals.push_back(normal);  
        } else if (prefix == "f") {  
            std::vector<int> vertex_indices, texture_indices, normal_indices;  
            std::string vertex_data;  

            while (iss >> vertex_data) {  
                std::istringstream viss(vertex_data);  
                std::string part;  
                int v_idx = -1, vt_idx = -1, vn_idx = -1;  

                // Parse vertex index  
                if (std::getline(viss, part, '/') && !part.empty()) {  
                    try {  
                        v_idx = resolve_index(std::stoi(part), model.vertices.size());  
                    } catch (...) {  
                        std::cerr << "Invalid vertex index: " << part << std::endl;  
                        return false;  
                    }  
                }  

                // Parse texture index  
                if (std::getline(viss, part, '/') && !part.empty()) {  
                    try {  
                        vt_idx = resolve_index(std::stoi(part), model.texture_coords.size());  
                    } catch (...) {  
                        std::cerr << "Invalid texture index: " << part << std::endl;  
                        return false;  
                    }  
                }  

                // Parse normal index  
                if (std::getline(viss, part, '/') && !part.empty()) {  
                    try {  
                        vn_idx = resolve_index(std::stoi(part), model.normals.size());  
                    } catch (...) {  
                        std::cerr << "Invalid normal index: " << part << std::endl;  
                        return false;  
                    }  
                }  

                vertex_indices.push_back(v_idx);  
                texture_indices.push_back(vt_idx);  
                normal_indices.push_back(vn_idx);  
            }  

            // Triangulate polygon faces (assumes convex)  
            for (size_t i = 1; i < vertex_indices.size() - 1; ++i) {  
                Face face;  
                face.vertex_index = { vertex_indices[0], vertex_indices[i], vertex_indices[i+1] }; 
                face.texture_index = { texture_indices[0], texture_indices[i], texture_indices[i+1] };  
                face.normal_index = { normal_indices[0], normal_indices[i], normal_indices[i+1] };  
                model.faces.push_back(face);  
            }  
        }  
    }  

    // Calculate tangents and bitangents (if UVs are present)  
    model.tangents.resize(model.vertices.size(), glm::vec3(0.0f));  
    model.bitangents.resize(model.vertices.size(), glm::vec3(0.0f));  

    bool has_uvs = !model.texture_coords.empty();  
    for (const auto &face : model.faces) {  
        if (!has_uvs || face.texture_index[0] < 0) continue;  

        glm::vec3 v0 = model.vertices[face.vertex_index[0]];  
        glm::vec3 v1 = model.vertices[face.vertex_index[1]];  
        glm::vec3 v2 = model.vertices[face.vertex_index[2]];  

        glm::vec2 uv0 = model.texture_coords[face.texture_index[0]];  
        glm::vec2 uv1 = model.texture_coords[face.texture_index[1]];  
        glm::vec2 uv2 = model.texture_coords[face.texture_index[2]];  

        glm::vec3 delta_pos1 = v1 - v0;  
        glm::vec3 delta_pos2 = v2 - v0;  
        glm::vec2 delta_uv1 = uv1 - uv0;  
        glm::vec2 delta_uv2 = uv2 - uv0;  

        float denom = delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x;  
        if (std::abs(denom) < 1e-6f) continue;  // Skip degenerate UVs  

        float r = 1.0f / denom;  
        glm::vec3 tangent = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;  
        glm::vec3 bitangent = (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r;  

        for (int i = 0; i < 3; ++i) {  
            int idx = face.vertex_index[i];  
            model.tangents[idx] += tangent;  
            model.bitangents[idx] += bitangent;  
        }  
    }  

    // Normalize tangents  
    for (size_t i = 0; i < model.vertices.size(); ++i) {  
        if (glm::length(model.tangents[i]) > 0.0f)  
            model.tangents[i] = glm::normalize(model.tangents[i]);  
        if (glm::length(model.bitangents[i]) > 0.0f)  
            model.bitangents[i] = glm::normalize(model.bitangents[i]);  
    }  

    return true;  
}  