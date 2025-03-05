#include "render.hpp"

bool load_shader(const std::string& vertex_path, const std::string& fragment_path, GLuint& shader_program) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    if (!vertex_file.is_open() || !fragment_file.is_open()) {
        std::cerr << "Failed to open shader files\n";
        return false;
    }

    std::stringstream vertex_stream, fragment_stream;
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    std::string vertex_code = vertex_stream.str();
    std::string fragment_code = fragment_stream.str();

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << "\n";
        return false;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << "\n";
        return false;
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << "\n";
        return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return true;
}

bool normals_mode = false;

void render_scene(RenderData& render_data, const ShaderUniformLocations& uniform_locations, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& camera_position, const LightProperties& light_properties) {
    glUseProgram(render_data.shader_program);

    glUniformMatrix4fv(uniform_locations.projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniform_locations.view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(uniform_locations.viewPos_loc, 1, glm::value_ptr(camera_position));

    glUniform3fv(uniform_locations.light_direction_loc, 1, glm::value_ptr(light_properties.direction));
    glUniform3fv(uniform_locations.light_ambient_loc, 1, glm::value_ptr(light_properties.ambient));
    glUniform3fv(uniform_locations.light_diffuse_loc, 1, glm::value_ptr(light_properties.diffuse));
    glUniform3fv(uniform_locations.light_specular_loc, 1, glm::value_ptr(light_properties.specular));

    for (auto &game_object : render_data.game_objects) {
        auto transform_component = game_object->get_component<TransformComponent>();
        if (!transform_component) {
            std::cerr << "GameObject `" << game_object->name << "` does not have a transform component (Which should never happen).\n";
            return;
        }

        glUniformMatrix4fv(uniform_locations.transform_loc, 1, GL_FALSE, glm::value_ptr(transform_component->get_transform()));

        auto material = game_object->get_component<MaterialComponent>();

        if (material) {
            glUniform3fv(uniform_locations.material_base_color_loc, 1, glm::value_ptr(material->material.base_color));
            glUniform1i(uniform_locations.material_base_map_loc, 0);
            glUniform1i(uniform_locations.material_metallic_map_loc, 1);
            glUniform1i(uniform_locations.material_normal_map_loc, 2);
            glUniform1i(uniform_locations.material_occlusion_map_loc, 4);
            glUniform1f(uniform_locations.material_smoothness_loc, material->material.smoothness);

            // Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->material.base_map);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material->material.metallic_map);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material->material.normal_map);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, material->material.occlusion_map);
        }

        auto render_mesh = game_object->get_component<RenderMeshComponent>();
        if (render_mesh) {
            glBindVertexArray(render_mesh->VAO);
            glDrawElements(GL_TRIANGLES, render_mesh->indices.size(), GL_UNSIGNED_INT, 0);

            if (normals_mode) {
                draw_normals(render_mesh, transform_component->get_transform(), projection, view);
            }
        }
    }
}

void draw_normals(const std::shared_ptr<RenderMeshComponent>& render_mesh, const glm::mat4& model, const glm::mat4& projection, const glm::mat4& view) {
    std::vector<float> normal_lines;
    for (size_t i = 0; i < render_mesh->vertices.size(); i += 8) {
        glm::vec3 pos(render_mesh->vertices[i], render_mesh->vertices[i + 1], render_mesh->vertices[i + 2]);
        glm::vec3 normal(render_mesh->vertices[i + 3], render_mesh->vertices[i + 4], render_mesh->vertices[i + 5]);

        glm::vec3 end = pos + normal * 0.1f; // Scale normal for visualization

        normal_lines.push_back(pos.x);
        normal_lines.push_back(pos.y);
        normal_lines.push_back(pos.z);
        normal_lines.push_back(end.x);
        normal_lines.push_back(end.y);
        normal_lines.push_back(end.z);
    }

    GLuint normal_VAO, normal_VBO;
    glGenVertexArrays(1, &normal_VAO);
    glGenBuffers(1, &normal_VBO);

    glBindVertexArray(normal_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, normal_lines.size() * sizeof(float), normal_lines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint normal_shader_program = create_normal_shader_program();
    glUseProgram(normal_shader_program);

    GLuint model_loc = glGetUniformLocation(normal_shader_program, "model");
    GLuint view_loc = glGetUniformLocation(normal_shader_program, "view");
    GLuint projection_loc = glGetUniformLocation(normal_shader_program, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(normal_VAO);
    glDrawArrays(GL_LINES, 0, normal_lines.size() / 3);

    glDeleteVertexArrays(1, &normal_VAO);
    glDeleteBuffers(1, &normal_VBO);
}

GLuint create_normal_shader_program() {
    const char* vertex_shader_source = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for normals
        }
    )";

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}