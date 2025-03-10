#ifndef RENDER_MESH_COMPONENT_HPP
#define RENDER_MESH_COMPONENT_HPP

#include "component.hpp"
#include "object_loader.hpp"
#include <glad/glad.h>
#include <vector>
#include <limits>
#include <unordered_map>
#include <functional>

class RenderMeshComponent : public Component {
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    size_t index_count = 0;

    RenderMeshComponent(const ObjModel &obj_model, bool generate_tangents = true) {
        compute_aabb(obj_model);
        process_mesh_data(obj_model, generate_tangents);
        setup_gl_buffers();
    }

    ~RenderMeshComponent() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    struct PackedVertex {
        int v_idx, vt_idx, vn_idx;
        bool operator==(const PackedVertex& o) const {
            return v_idx == o.v_idx && vt_idx == o.vt_idx && vn_idx == o.vn_idx;
        }
    };

    struct VertexHasher {
        size_t operator()(const PackedVertex& k) const {
            return ((std::hash<int>()(k.v_idx) ^
                   (std::hash<int>()(k.vt_idx) << 1) >> 1) ^
                   (std::hash<int>()(k.vn_idx) << 1));
        }
    };

    void compute_aabb(const ObjModel &obj_model) {
        aabb_min = glm::vec3(FLT_MAX);
        aabb_max = glm::vec3(-FLT_MAX);
        for (const auto& v : obj_model.vertices) {
            aabb_min = glm::min(aabb_min, v);
            aabb_max = glm::max(aabb_max, v);
        }
    }

    void process_mesh_data(const ObjModel &obj_model, bool generate_tangents) {
        std::unordered_map<PackedVertex, unsigned int, VertexHasher> vertex_map;
        std::vector<glm::vec3> temp_tangents;
        std::vector<glm::vec3> temp_bitangents;

        // First pass: Process all faces and build unique vertices
        for (const auto& face : obj_model.faces) {
            glm::vec3 pos[3];
            glm::vec2 uv[3];
            glm::vec3 normal[3];

            // Get vertex data for this face
            for (int i = 0; i < 3; i++) {
                pos[i] = obj_model.vertices[face.vertex_index[i]];
                uv[i] = face.texture_index[i] >= 0 ? 
                       obj_model.texture_coords[face.texture_index[i]] : glm::vec2(0);
                normal[i] = face.normal_index[i] >= 0 ? 
                           obj_model.normals[face.normal_index[i]] : glm::vec3(0);
            }

            // Generate tangent/bitangent if needed
            if (generate_tangents && !obj_model.texture_coords.empty()) {
                glm::vec3 tangent, bitangent;
                compute_face_tangents(pos, uv, tangent, bitangent);
                temp_tangents.push_back(tangent);
                temp_bitangents.push_back(bitangent);
            }

            // Process each vertex in the face
            for (int i = 0; i < 3; i++) {
                PackedVertex packed = {
                    face.vertex_index[i],
                    face.texture_index[i],
                    face.normal_index[i]
                };

                // Check if we've already seen this vertex
                auto it = vertex_map.find(packed);
                if (it != vertex_map.end()) {
                    indices.push_back(it->second);
                } else {
                    // Create new vertex
                    const unsigned int new_idx = vertices.size() / 14; // 14 floats per vertex
                    vertex_map[packed] = new_idx;
                    indices.push_back(new_idx);

                    // Position
                    vertices.push_back(pos[i].x);
                    vertices.push_back(pos[i].y);
                    vertices.push_back(pos[i].z);

                    // Normal
                    vertices.push_back(normal[i].x);
                    vertices.push_back(normal[i].y);
                    vertices.push_back(normal[i].z);

                    // Texture coordinates
                    vertices.push_back(uv[i].x);
                    vertices.push_back(uv[i].y);

                    // Tangent/Bitangent (if generated)
                    if (generate_tangents && !temp_tangents.empty()) {
                        vertices.push_back(temp_tangents.back().x);
                        vertices.push_back(temp_tangents.back().y);
                        vertices.push_back(temp_tangents.back().z);
                        vertices.push_back(temp_bitangents.back().x);
                        vertices.push_back(temp_bitangents.back().y);
                        vertices.push_back(temp_bitangents.back().z);
                    } else {
                        // Pad with zeros
                        vertices.insert(vertices.end(), 6, 0.0f);
                    }
                }
            }
        }
        index_count = indices.size();
    }

    void compute_face_tangents(const glm::vec3 pos[3], const glm::vec2 uv[3], 
                              glm::vec3& tangent, glm::vec3& bitangent) {
        glm::vec3 edge1 = pos[1] - pos[0];
        glm::vec3 edge2 = pos[2] - pos[0];
        glm::vec2 deltaUV1 = uv[1] - uv[0];
        glm::vec2 deltaUV2 = uv[2] - uv[0];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
    }

    void setup_gl_buffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        
        // Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Index Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Vertex Attributes
        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        
        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // Texture Coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        
        // Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        
        // Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

        glBindVertexArray(0);
    }
};

#endif // RENDER_MESH_COMPONENT_HPP