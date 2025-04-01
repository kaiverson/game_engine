#include "mesh.hpp"

Mesh::Mesh() {}

void Mesh::set_vertices(const std::vector<Vertex> &vertices) {
    this->vertices = vertices;
    is_uploaded = false; 
}

void Mesh::set_indices(const std::vector<GLuint> &indices) {
    this->indices = indices;
}

bool Mesh::add_submesh(GLuint index_offset, GLuint index_count) {
    if (index_offset + index_count > indices.size()) {
        std::cout << "Mesh: added submesh with indices out of range of the index buffer\n";
        return false;
    }

    submeshes.push_back({index_offset, index_count});
    return true;
}

void Mesh::upload_to_GPU() {
    if (is_uploaded) return;

    // Generate buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Bind vao
    glBindVertexArray(vao);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


    // Set vertex attributes
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // tangent
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(2);

    // uv0
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv0));
    glEnableVertexAttribArray(3);

    // uv1
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv1));
    glEnableVertexAttribArray(4);


    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    is_uploaded = true;
}

bool Mesh::bind() const {
    if (!is_uploaded) return false;

    glBindVertexArray(vao);
    return true;
}

bool Mesh::draw_submesh(size_t submesh_index) const {
    if (!is_uploaded || submesh_index >= submeshes.size()) return false;

    const Submesh &submesh = submeshes[submesh_index];
    glDrawElements(GL_TRIANGLES, submesh.index_count, GL_UNSIGNED_INT, (void*)(submesh.index_offset * sizeof(GLuint)));
    return true;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}


bool Mesh::load(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |  // Generates tangents/bitangents
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Clear existing data
    vertices.clear();
    indices.clear();
    submeshes.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* ai_mesh = scene->mMeshes[i];
        const size_t vertex_offset = vertices.size();
        const size_t index_offset = indices.size();

        // Process vertices
        for (unsigned int j = 0; j < ai_mesh->mNumVertices; ++j) {
            Vertex vertex;

            // Position
            vertex.position = {
                ai_mesh->mVertices[j].x,
                ai_mesh->mVertices[j].y,
                ai_mesh->mVertices[j].z
            };

            // Normal
            vertex.normal = {
                ai_mesh->mNormals[j].x,
                ai_mesh->mNormals[j].y,
                ai_mesh->mNormals[j].z
            };

            // Tangent (vec4 with handedness in .w)
            if (ai_mesh->mTangents) {
                vertex.tangent = {
                    ai_mesh->mTangents[j].x,
                    ai_mesh->mTangents[j].y,
                    ai_mesh->mTangents[j].z,
                    1.0f  // Default handedness (adjusted below if bitangents exist)
                };

                // Calculate correct handedness using bitangent
                if (ai_mesh->mBitangents) {
                    const glm::vec3 normal(vertex.normal);
                    const glm::vec3 tangent(vertex.tangent);
                    const glm::vec3 bitangent(
                        ai_mesh->mBitangents[j].x,
                        ai_mesh->mBitangents[j].y,
                        ai_mesh->mBitangents[j].z
                    );

                    const glm::vec3 computed_bitangent = glm::cross(normal, tangent);
                    vertex.tangent.w = glm::dot(computed_bitangent, bitangent) > 0.0f ? 1.0f : -1.0f;
                }
            } else {
                vertex.tangent = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }

            // UV0 (primary texture coordinates)
            if (ai_mesh->mTextureCoords[0]) {
                vertex.uv0 = {
                    ai_mesh->mTextureCoords[0][j].x,
                    ai_mesh->mTextureCoords[0][j].y
                };
            } else {
                vertex.uv0 = {0.0f, 0.0f};
            }

            // UV1 (secondary texture coordinates, e.g., lightmaps)
            if (ai_mesh->mTextureCoords[1]) {
                vertex.uv1 = {
                    ai_mesh->mTextureCoords[1][j].x,
                    ai_mesh->mTextureCoords[1][j].y
                };
            } else {
                vertex.uv1 = {0.0f, 0.0f};
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int j = 0; j < ai_mesh->mNumFaces; ++j) {
            const aiFace& face = ai_mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                indices.push_back(face.mIndices[k] + vertex_offset);
            }
        }

        // Add submesh
        submeshes.push_back({
            static_cast<GLuint>(index_offset),
            static_cast<GLuint>(ai_mesh->mNumFaces * 3)
        });
    }

    return true;
}