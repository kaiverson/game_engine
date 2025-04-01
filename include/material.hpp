#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "shader.hpp"
#include "texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>
#include <string>


enum class BlendMode { Opaque, AlphaBlend, Additive };
enum class CullMode {None, Back, Front };

using UniformValue = std::variant<float, glm::vec3, glm::mat4>;

struct Uniform {
    GLuint location;
    UniformValue value;
};

struct TextureUniform {
    GLuint location;
    std::shared_ptr<Texture> texture;
    int unit;
};

class Material {
private:
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, Uniform> uniforms;
    std::unordered_map<std::string, TextureUniform> texture_uniforms;
    int next_texture_unit = 0;

    BlendMode blend_mode = BlendMode::Opaque;
    bool depth_test = true;
    bool depth_write = true;
    CullMode cull_mode = CullMode::Back;

public:
    void set_shader(std::shared_ptr<Shader> shader);
    bool set_uniform(const std::string &name, float value);
    bool set_uniform(const std::string &name, glm::vec3 value);
    bool set_uniform(const std::string &name, glm::mat4 value);
    bool set_uniform(const std::string &name, std::shared_ptr<Texture> texture);

    void list_all_uniforms();
    void check_uniforms();

    void set_blend_mode(BlendMode mode);
    void set_depth_test(bool enable);
    void set_depth_write(bool enable);
    void set_cull_mode(CullMode mode);
    void apply();
};

#endif // MATERIAL_HPP