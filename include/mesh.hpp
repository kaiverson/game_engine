#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <memory>
#include <string>

class Mesh {
private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 tangent;
        glm::vec2 uv0;
        glm::vec2 uv1;
    };

    struct Submesh {
        GLuint index_offset;
        GLuint index_count;
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Submesh> submeshes;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    bool is_uploaded = false;

public:
    Mesh();
    void set_vertices(const std::vector<Vertex> &vertices);
    void set_indices(const std::vector<GLuint> &indices);
    bool add_submesh(GLuint index_offset, GLuint index_count);
    void upload_to_GPU();
    bool bind() const;
    bool draw_submesh(size_t submesh_index) const;
    size_t get_submesh_count() const { return submeshes.size(); }
    ~Mesh();

    // Supports .obj and .fbx fils
    bool load(const std::string &path);
};

#endif // MESH_HPP