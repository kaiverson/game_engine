#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.hpp"

class Skybox {
public:
    Skybox();
    Skybox(const std::vector<std::string>& faces, GLuint shader);
    ~Skybox();
    bool render(const glm::mat4& view, const glm::mat4& projection) const;

public:
    GLuint VAO, VBO;
    GLuint texture;
    GLuint shader;

private:
    bool loaded = false;

    GLint view_uniform_loc;
    GLint projection_uniform_loc;
    GLint skybox_uniform_loc;
};