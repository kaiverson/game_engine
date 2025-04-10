#include "material.hpp"

namespace {
    void log_uniform_not_found(const std::string& name) {
        std::cerr << "Material: Uniform '" << name << "' not found in shader program.\n";
    }
}

//////////////////////////////
// Shader/Uniform Functions //
//////////////////////////////

// Associates a shader with the material and resets texture unit assignment.
void Material::set_shader(std::shared_ptr<Shader> shader) {
    this->shader = shader;
    next_texture_unit = 0;
}

// set float uniform; Logs error if not found.
bool Material::set_uniform(const std::string &name, float value) {
    GLuint location = glGetUniformLocation(shader->get_program(), name.c_str());
    if (location == static_cast<GLuint>(-1)) {
        log_uniform_not_found(name);
        return false;
    }

    uniforms[name] = {location, value}; 
    return true;
}

// set glm::vec3 uniform; Logs error if not found.
bool Material::set_uniform(const std::string& name, glm::vec3 value) {
    GLuint location = glGetUniformLocation(shader->get_program(), name.c_str());
    if (location == static_cast<GLuint>(-1)) {
        log_uniform_not_found(name);
        return false;
    }
    uniforms[name] = {location, value}; 
    return true;
}

// set glm::mat4 uniform; logs errors if not found.
bool Material::set_uniform(const std::string& name, glm::mat4 value) {
    GLuint location = glGetUniformLocation(shader->get_program(), name.c_str());
    if (location == static_cast<GLuint>(-1)) {
        log_uniform_not_found(name);
        return false;
    }
    uniforms[name] = {location, value}; 
    return true;
}

// Sets a texture uniform, assigns it a texture unit automatically.
bool Material::set_uniform(const std::string& name, std::shared_ptr<Texture> texture) {
    GLuint location = glGetUniformLocation(shader->get_program(), name.c_str());
    if (location == static_cast<GLuint>(-1)) {
        log_uniform_not_found(name);
        return false;
    }
    int unit = next_texture_unit++;
    texture_uniforms[name] = {location, texture, unit}; 
    return true;
}


/////////////////////////////
// Utility/Debug Functions //
/////////////////////////////

// Returns a human-readable string for a given GL type.
std::string get_type_string(GLenum type) {
    std::string type_string;
    switch (type) {
        case GL_FLOAT:         type_string = "float"; break;
        case GL_FLOAT_VEC2:    type_string = "vec2"; break;
        case GL_FLOAT_VEC3:    type_string = "vec3"; break;
        case GL_FLOAT_VEC4:    type_string = "vec4"; break;
        case GL_INT:           type_string = "int"; break;
        case GL_INT_VEC2:      type_string = "ivec2"; break;
        case GL_INT_VEC3:      type_string = "ivec3"; break;
        case GL_INT_VEC4:      type_string = "ivec4"; break;
        case GL_BOOL:          type_string = "bool"; break;
        case GL_BOOL_VEC2:     type_string = "bvec2"; break;
        case GL_BOOL_VEC3:     type_string = "bvec3"; break;
        case GL_BOOL_VEC4:     type_string = "bvec4"; break;
        case GL_FLOAT_MAT2:    type_string = "mat2"; break;
        case GL_FLOAT_MAT3:    type_string = "mat3"; break;
        case GL_FLOAT_MAT4:    type_string = "mat4"; break;
        case GL_SAMPLER_2D:    type_string = "sampler2D"; break;
        case GL_SAMPLER_CUBE:  type_string = "samplerCube"; break;
        default:               type_string = "unknown"; break;
    }
    return type_string;
}

// Prints all active uniforms in the shader for debugging purposes.
void Material::print_all_uniforms() {
    GLint num_uniforms = 0;
    glGetProgramiv(shader->get_program(), GL_ACTIVE_UNIFORMS, &num_uniforms);

    std::cout << "Number of uniforms: " << num_uniforms << "\n";

    for (GLint i = 0; i < num_uniforms; ++i) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(shader->get_program(), i, sizeof(name), &length, &size, &type, name);
        std::cout << "Uniform #" << i << ": " << get_type_string(type) << " " << name << "\n";
    }
    std::cout << "\n";
}

// Checks that every active uniform in the shader has been set; logs warnings otherwise.
void Material::check_uniforms() {
    GLint num_uniforms = 0;
    glGetProgramiv(shader->get_program(), GL_ACTIVE_UNIFORMS, &num_uniforms);
    std::unordered_map<std::string, std::pair<bool, GLenum>> uniform_set_map;

    // build a map of shader uniforms.
    for (GLint i = 0; i < num_uniforms; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(shader->get_program(), i, sizeof(name), &length, &size, &type, name);
        uniform_set_map[name] = {false, type};
    }

    // Mark uniforms set via the regular uniform map.
    for (const auto& [name, uniform] : uniforms) {
        if (uniform_set_map.find(name) != uniform_set_map.end()) {
            uniform_set_map[name].first = true;
        }
    }

    for (const auto &[name, texture_uniform] : texture_uniforms) {
        if (uniform_set_map.find(name) != uniform_set_map.end()) {
            uniform_set_map[name].first = true;
        }
    }

    bool all_set = true;
    for (const auto &[name, pair] : uniform_set_map) {
        const auto &[is_set, type] = pair;
        if (!is_set) {
            std::cerr << "Warning: uniform " << get_type_string(type) << name << " has not been set.\n";
            all_set = false;
        }
    }
    if (all_set) {
        std::cout << "All uniforms set\n";
    }

    std::cout << "\n";
}

/////////////////////////////
// Render State Management //
/////////////////////////////

void Material::set_blend_mode(BlendMode mode) { blend_mode = mode; }
void Material::set_depth_test(bool enable) { depth_test = enable; }
void Material::set_depth_write(bool enable) { depth_write = enable; }
void Material::set_cull_mode(CullMode mode) { cull_mode = mode; }

// Applies the material: sets rendering states and updates all uniforms and textures.
void Material::apply() {
    // Set blend mode. 
    switch (blend_mode) {
        case BlendMode::Opaque:
            glDisable(GL_BLEND);
            break;
        case BlendMode::AlphaBlend:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
    }

    // Set depth test
    depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(depth_write ? GL_TRUE : GL_FALSE);

    // Set face culling
    switch (cull_mode) {
        case CullMode::None:
            glDisable(GL_CULL_FACE);
            break;
        case CullMode::Back:
            glEnable(GL_CULL_FACE); 
            glCullFace(GL_BACK);
            break;
        case CullMode::Front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
    }

    // Activate shader
    shader->use();

    // Upload regular uniforms.
    for (const auto &[name, uniform] : uniforms) {
        std::visit([&](auto &&value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, float>) {
                glUniform1f(uniform.location, value);

            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                glUniform3fv(uniform.location, 1, glm::value_ptr(value));

            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));

            }
        }, uniform.value);
    }

    // Bind and update texture uniforms.
    for (const auto &[name, texture_uniform] : texture_uniforms) {
        glActiveTexture(GL_TEXTURE0 + texture_uniform.unit);
        texture_uniform.texture->bind();
        glUniform1i(texture_uniform.location, texture_uniform.unit);
    }
}

///////////////////////////
// ImGui Debug/Editor UI //
///////////////////////////

// Displays and allows editing of uniforms via ImGui.
// Displays and allows editing of uniforms via ImGui.
void Material::draw_uniforms_gui() {
    // Iterate over basic uniforms.
    for (auto& [name, uniform] : uniforms) {
        ImGui::Text("Uniform: %s", name.c_str());
        if (std::holds_alternative<float>(uniform.value)) {
            float& value = std::get<float>(uniform.value);
            if (ImGui::SliderFloat(name.c_str(), &value, 0.0f, 1.0f)) {
                set_uniform(name, value);
            }
        } else if (std::holds_alternative<glm::vec3>(uniform.value)) {
            glm::vec3& value = std::get<glm::vec3>(uniform.value);
            if (ImGui::ColorEdit3(name.c_str(), glm::value_ptr(value))) {
                set_uniform(name, value);
            }
        } else if (std::holds_alternative<glm::mat4>(uniform.value)) {
            // TODO: Implement a proper matrix editor.
            ImGui::Text("Matrix4x4 editing not implemented");
        }
    }
    
    // Iterate over texture uniforms.
    for (auto& [name, texture_uniform] : texture_uniforms) {
        ImGui::Text("Texture Uniform: %s", name.c_str());
        if (texture_uniform.texture) {
            ImGui::Text("Texture Unit: %d", texture_uniform.unit);
            // TODO: Add UI control to change or preview the texture.
            ImGui::Text("Texture editing not implemented.");
        } else {
            ImGui::Text("No texture assigned");
        }
    }
}