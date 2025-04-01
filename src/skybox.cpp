#include "skybox.hpp"

Skybox::Skybox() 
    : VAO(0), VBO(0), texture(0), shader(0), loaded(false)
{}

Skybox::Skybox(const std::string& directory, std::shared_ptr<Shader> shader) 
        : shader(shader) {
    // float skyboxVertices[] = {
    //     // positions          
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,

    //     -1.0f, -1.0f,  1.0f,
    //     -1.0f, -1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //     -1.0f, -1.0f,  1.0f,

    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,

    //     -1.0f, -1.0f,  1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f, -1.0f,  1.0f,
    //     -1.0f, -1.0f,  1.0f,

    //     -1.0f,  1.0f, -1.0f,
    //      1.0f,  1.0f, -1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //     -1.0f,  1.0f, -1.0f,

    //     -1.0f, -1.0f, -1.0f,
    //     -1.0f, -1.0f,  1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //     -1.0f, -1.0f,  1.0f,
    //      1.0f, -1.0f,  1.0f
    // };

    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);

    // if (VAO == 0 || VBO == 0) {
    //     std::cerr << "Skybox: Failed to generate OpenGL objects\n";
    //     return;
    // }

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glBindVertexArray(0);

    // std::vector<std::string> faces = {
    //     directory + "/right.png",
    //     directory + "/left.png",
    //     directory + "/top.png",
    //     directory + "/bottom.png",
    //     directory + "/front.png",
    //     directory + "/back.png"
    // };

    // texture = load_cubemap(faces);

    // view_uniform_loc = glGetUniformLocation(shader, "view");
    // projection_uniform_loc = glGetUniformLocation(shader, "projection");
    // skybox_uniform_loc = glGetUniformLocation(shader, "skybox");

    // if(view_uniform_loc == -1 || projection_uniform_loc == -1 || skybox_uniform_loc == -1) {
    //     std::cerr << "Skybox: Failed to find one or more uniform locations\n";
    //     return;
    // }

    // loaded = true;
}

Skybox::~Skybox() {
    // if (!loaded) return;

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteTextures(1, &texture);
}

bool Skybox::render(const glm::mat4& view, const glm::mat4& projection) const {
    // if (!loaded || shader == 0) return false;

    // glDepthFunc(GL_LEQUAL);
    // glUseProgram(shader);

    // // Remove translation from the view matrix
    // glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));

    // glUniformMatrix4fv(view_uniform_loc, 1, GL_FALSE, glm::value_ptr(view_no_translation));
    // glUniformMatrix4fv(projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(projection));
    // glUniform1i(skybox_uniform_loc, 0);

    // glBindVertexArray(VAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // // Clearnup
    // glBindVertexArray(0);
    // glDepthFunc(GL_LESS);

    return true;
}