#include "skybox.hpp"

Skybox::Skybox() {}

Skybox::Skybox(const std::vector<std::string>& faces, GLuint shader) {
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    texture = load_cubemap(faces);

    view_uniform_loc = glGetUniformLocation(shader, "view");
    if (view_uniform_loc == -1) {
        std::cerr << "Could not find skybox uniform location for: view\n"; 
    }
    projection_uniform_loc = glGetUniformLocation(shader, "projection");
    if (view_uniform_loc == -1) {
        std::cerr << "Could not find skybox uniform location for: projection\n"; 
    }
    skybox_uniform_loc = glGetUniformLocation(shader, "skybox");
    if (skybox_uniform_loc == -1) {
        std::cerr << "Could not find skybox uniform location for: skybox\n"; 
    }

    loaded = true;
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
}

bool Skybox::render(const glm::mat4& view, const glm::mat4& projection) const {
    if (!loaded) {
        return false;
    }

    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader);

    // Remove translation from the view matrix
    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));

    glUniformMatrix4fv(view_uniform_loc, 1, GL_FALSE, glm::value_ptr(view_no_translation));
    glUniformMatrix4fv(projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(skybox_uniform_loc, 0);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    return true;
}