#include "object_loader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <glm/glm.hpp>

bool load_obj(const std::string &filename, ObjModel &model) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

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
            model.texture_coords.push_back(tex_coord);
        } else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            model.normals.push_back(normal);
        } else if (prefix == "f") {
            std::vector<int> vertex_indices;
            std::vector<int> texture_indices;
            std::vector<int> normal_indices;
            char slash;
            std::string vertex_data;

            while (iss >> vertex_data) {
                std::istringstream viss(vertex_data);
                std::string index;
                int vertex_index = -1, texture_index = -1, normal_index = -1;

                std::getline(viss, index, '/');
                if (!index.empty()) vertex_index = std::stoi(index) - 1;

                if (viss.peek() == '/') viss.ignore();
                if (viss.peek() != '/') {
                    std::getline(viss, index, '/');
                    if (!index.empty()) texture_index = std::stoi(index) - 1;
                }

                if (viss.peek() == '/') viss.ignore();
                std::getline(viss, index);
                if (!index.empty()) normal_index = std::stoi(index) - 1;
                
                vertex_indices.push_back(vertex_index);
                texture_indices.push_back(texture_index);
                normal_indices.push_back(normal_index);
            }
            
            for (size_t i = 1; i < vertex_indices.size() - 1; i++) {
                Face face;
                face.vertex_index[0] = vertex_indices[0];
                face.vertex_index[1] = vertex_indices[i];
                face.vertex_index[2] = vertex_indices[i + 1];

                face.texture_index[0] = texture_indices[0];
                face.texture_index[1] = texture_indices[i];
                face.texture_index[2] = texture_indices[i + 1];

                face.normal_index[0] = normal_indices[0];
                face.normal_index[1] = normal_indices[i];
                face.normal_index[2] = normal_indices[i + 1];

                model.faces.push_back(face);
            }
        }
    }

    file.close();

    // Calculate tangents and bitangents
    model.tangents.resize(model.vertices.size(), glm::vec3(0.0f));
    model.bitangents.resize(model.vertices.size(), glm::vec3(0.0f));

    for (const auto &face : model.faces) {
        glm::vec3 v0 = model.vertices.at(face.vertex_index[0]);
        glm::vec3 v1 = model.vertices.at(face.vertex_index[1]);
        glm::vec3 v2 = model.vertices.at(face.vertex_index[2]);

        if (face.texture_index[0] >= 0 && face.texture_index[1] >= 0 && face.texture_index[2] >= 0) {
            glm::vec2 uv0 = model.texture_coords[face.texture_index[0]];
            glm::vec2 uv1 = model.texture_coords[face.texture_index[1]];
            glm::vec2 uv2 = model.texture_coords[face.texture_index[2]];

            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            model.tangents[face.vertex_index[0]] += tangent;
            model.tangents[face.vertex_index[1]] += tangent;
            model.tangents[face.vertex_index[2]] += tangent;

            model.bitangents[face.vertex_index[0]] += bitangent;
            model.bitangents[face.vertex_index[1]] += bitangent;
            model.bitangents[face.vertex_index[2]] += bitangent;
        }
    }

    // Normalize tangents and bitangents
    for (size_t i = 0; i < model.vertices.size(); ++i) {
        model.tangents[i] = glm::normalize(model.tangents[i]);
        model.bitangents[i] = glm::normalize(model.bitangents[i]);
    }

    return true;
}