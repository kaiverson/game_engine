#ifndef RENDER_MESH_COMPONENT_HPP
#define RENDER_MESH_COMPONENT_HPP

#include "component.hpp"
#include "object_loader.hpp"
#include <glad/glad.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

class RenderMeshComponent : public Component {
public:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    RenderMeshComponent(const ObjModel &obj_model) {
        bool has_normals = !obj_model.normals.empty();
        bool has_texcoords = !obj_model.texture_coords.empty();

        std::unordered_map<int, glm::vec3> vertex_normals;
        std::unordered_map<int, glm::vec3> vertex_tangents;
        std::unordered_map<int, glm::vec3> vertex_bitangents;

        // Initialize vertex_normals, vertex_tangents, and vertex_bitangents with zero vectors
        for (size_t i = 0; i < obj_model.vertices.size(); ++i) {
            vertex_normals[i] = glm::vec3(0.0f);
            vertex_tangents[i] = glm::vec3(0.0f);
            vertex_bitangents[i] = glm::vec3(0.0f);
        }

        for (const auto &face : obj_model.faces) {
            glm::vec3 v0 = obj_model.vertices[face.vertex_index[0]];
            glm::vec3 v1 = obj_model.vertices[face.vertex_index[1]];
            glm::vec3 v2 = obj_model.vertices[face.vertex_index[2]];

            glm::vec2 uv0 = obj_model.texture_coords[face.texture_index[0]];
            glm::vec2 uv1 = obj_model.texture_coords[face.texture_index[1]];
            glm::vec2 uv2 = obj_model.texture_coords[face.texture_index[2]];

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;

            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * r;

            for (int i = 0; i < 3; ++i) {
                vertex_normals[face.vertex_index[i]] += glm::normalize(glm::cross(edge1, edge2));
                vertex_tangents[face.vertex_index[i]] += tangent;
                vertex_bitangents[face.vertex_index[i]] += bitangent;
            }

            for (int i = 0; i < 3; ++i) {
                indices.push_back(indices.size());
                vertices.push_back(obj_model.vertices[face.vertex_index[i]].x);
                vertices.push_back(obj_model.vertices[face.vertex_index[i]].y);
                vertices.push_back(obj_model.vertices[face.vertex_index[i]].z);

                if (has_normals) {
                    vertices.push_back(obj_model.normals[face.normal_index[i]].x);
                    vertices.push_back(obj_model.normals[face.normal_index[i]].y);
                    vertices.push_back(obj_model.normals[face.normal_index[i]].z);
                } else {
                    vertices.push_back(vertex_normals[face.vertex_index[i]].x);
                    vertices.push_back(vertex_normals[face.vertex_index[i]].y);
                    vertices.push_back(vertex_normals[face.vertex_index[i]].z);
                }

                if (has_texcoords) {
                    vertices.push_back(obj_model.texture_coords[face.texture_index[i]].x);
                    vertices.push_back(obj_model.texture_coords[face.texture_index[i]].y);
                } else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

                vertices.push_back(vertex_tangents[face.vertex_index[i]].x);
                vertices.push_back(vertex_tangents[face.vertex_index[i]].y);
                vertices.push_back(vertex_tangents[face.vertex_index[i]].z);

                vertices.push_back(vertex_bitangents[face.vertex_index[i]].x);
                vertices.push_back(vertex_bitangents[face.vertex_index[i]].y);
                vertices.push_back(vertex_bitangents[face.vertex_index[i]].z);
            }
        }

        // Normalize vertex normals, tangents, and bitangents
        for (auto &vertex_normal : vertex_normals) {
            vertex_normal.second = glm::normalize(vertex_normal.second);
        }
        for (auto &vertex_tangent : vertex_tangents) {
            vertex_tangent.second = glm::normalize(vertex_tangent.second);
        }
        for (auto &vertex_bitangent : vertex_bitangents) {
            vertex_bitangent.second = glm::normalize(vertex_bitangent.second);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set up position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Set up normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Set up texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Set up tangent attribute
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);

        // Set up bitangent attribute
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
        glEnableVertexAttribArray(4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~RenderMeshComponent() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void start(GameObject &game_object) override {
        
    }

    void update(GameObject& gameObject) override {
        // No update logic needed for the mesh component
    }
};

#endif // RENDER_MESH_COMPONENT_HPP