#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include <shader.hpp>
#include <texture.hpp>

class Skybox {
public:
    Skybox();
    Skybox(const std::string &directory, std::shared_ptr<Shader> shader);
    ~Skybox();
    bool render(const glm::mat4& view, const glm::mat4& projection) const;

public:
    GLuint VAO, VBO;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> shader;

private:
    bool loaded = false;

    GLint view_uniform_loc;
    GLint projection_uniform_loc;
    GLint skybox_uniform_loc;
};