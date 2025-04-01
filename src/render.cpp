#include "render.hpp"

bool normals_mode = false;

void render_scene(const Scene& scene, const float aspect_ratio) {
    bool success = true;
    
    auto camera = scene.get_main_camera();
    if (!camera) {
        std::cerr << "Render: No main camera\n";
        return;
    }

    LightProperties light_properties;
    light_properties.direction = glm::vec3(1.2f, -15.0f, 2.0f);
    light_properties.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light_properties.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light_properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    auto camera_transform = scene.get_main_camera()->get_component<TransformComponent>();
    auto camera_position = camera_transform->position;

    auto camera_component = scene.get_main_camera_component();
    glm::mat4 projection = camera_component->get_projection_matrix(aspect_ratio);
    glm::mat4 view = camera_component->get_view_matrix(camera_transform->position, camera_transform->get_front(), camera_transform->get_up());

    camera_component->set_viewport();
    camera_component->clear(view, projection);

    for (auto &game_object : scene.get_game_objects()) {
        auto render_mesh_component = game_object->get_component<RenderMeshComponent>();
        if (!render_mesh_component) {
            continue;
        }

        success = render_mesh_component->render(projection, view, camera_position, light_properties);
        if (!success) {
            std::cerr << "Render: '" << game_object->name << "' failed to render\n";
        }
    }
}

// void draw_normals(const std::shared_ptr<RenderMeshComponent>& render_mesh, const glm::mat4& model, const glm::mat4& projection, const glm::mat4& view) {
//     std::vector<float> normal_lines;
//     for (size_t i = 0; i < render_mesh->vertices.size(); i += 14) {
//         glm::vec3 pos(render_mesh->vertices[i], render_mesh->vertices[i + 1], render_mesh->vertices[i + 2]);
//         glm::vec3 normal(render_mesh->vertices[i + 3], render_mesh->vertices[i + 4], render_mesh->vertices[i + 5]);

//         glm::vec3 end = pos + normal * 0.1f; // Scale normal for visualization

//         normal_lines.push_back(pos.x);
//         normal_lines.push_back(pos.y);
//         normal_lines.push_back(pos.z);
//         normal_lines.push_back(end.x);
//         normal_lines.push_back(end.y);
//         normal_lines.push_back(end.z);
//     }

//     GLuint normal_VAO, normal_VBO;
//     glGenVertexArrays(1, &normal_VAO);
//     glGenBuffers(1, &normal_VBO);

//     glBindVertexArray(normal_VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
//     glBufferData(GL_ARRAY_BUFFER, normal_lines.size() * sizeof(float), normal_lines.data(), GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     GLuint normal_shader_program = create_normal_shader_program();
//     glUseProgram(normal_shader_program);

//     GLuint model_loc = glGetUniformLocation(normal_shader_program, "model");
//     GLuint view_loc = glGetUniformLocation(normal_shader_program, "view");
//     GLuint projection_loc = glGetUniformLocation(normal_shader_program, "projection");

//     glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
//     glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
//     glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

//     glBindVertexArray(normal_VAO);
//     glDrawArrays(GL_LINES, 0, normal_lines.size() / 3);

//     glDeleteVertexArrays(1, &normal_VAO);
//     glDeleteBuffers(1, &normal_VBO);
// }

// GLuint create_normal_shader_program() {
//     const char* vertex_shader_source = R"(
//         #version 330 core
//         layout(location = 0) in vec3 aPos;
//         uniform mat4 model;
//         uniform mat4 view;
//         uniform mat4 projection;
//         void main() {
//             gl_Position = projection * view * model * vec4(aPos, 1.0);
//         }
//     )";

//     const char* fragment_shader_source = R"(
//         #version 330 core
//         out vec4 FragColor;
//         void main() {
//             FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for normals
//         }
//     )";

//     GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
//     glCompileShader(vertex_shader);

//     GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
//     glCompileShader(fragment_shader);

//     GLuint shader_program = glCreateProgram();
//     glAttachShader(shader_program, vertex_shader);
//     glAttachShader(shader_program, fragment_shader);
//     glLinkProgram(shader_program);

//     glDeleteShader(vertex_shader);
//     glDeleteShader(fragment_shader);

//     return shader_program;
// }