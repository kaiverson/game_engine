#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

#include "object_loader.hpp"
#include "utils.hpp"
#include "shader_utils.hpp"
#include "callbacks.hpp"
#include "render.hpp"
#include "game_object.hpp"
#include "game_object_builder.hpp"
#include "components/transform_component.hpp"
#include "components/material_component.hpp"
#include "components/render_mesh_component.hpp"
#include "components/script_component.hpp"
#include "components/camera_component.hpp"
#include "input_state.hpp"
#include "scene.hpp"
#include "skybox.hpp"

#include "change_color_script.hpp"
#include "camera_movement_script.hpp"

bool wireframe_mode = false;
bool debug_mode = false;

GLuint SCREEN_WIDTH = 1000;
GLuint SCREEN_HEIGHT = 800;

const int TARGET_FPS = 120;
const double FRAME_DURATION = 1.0 / TARGET_FPS;

std::shared_ptr<GameObject> camera_object;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GAME", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create Scene
    Scene main_scene;

    // Load shaders
    GLuint shaderProgram;
    if (!load_shader("src/shaders/utah/vertex_shader.glsl", "src/shaders/utah/fragment_shader.glsl", shaderProgram)) {
        std::cerr << "Failed to load shaders\n";
        return -1;
    }

    auto ground = main_scene.create_game_object("Ground")
        .with_render_mesh("src/objects/plane.obj")
        .with_material()
            .set_shader(shaderProgram)
            .with_texture("src/textures/ground.jpg")
            // .with_texture("src/textures/ground.jpg", TextureType::Normal)
        .with_transform(glm::vec3(0), glm::vec3(3.0f))
        .build();
    
    auto teapot = main_scene.create_game_object("Trees")
        .with_render_mesh("src/objects/UTAH_BLEND.obj")
        .with_material()
            .set_shader(shaderProgram)
            .with_texture("src/objects/dragon/Material_baseColor.png")
            .with_texture("src/objects/dragon/Material_normal.png", TextureType::Normal)
        .build();


    GLuint skybox_shader;
    if (!load_shader("src/shaders/skybox/skybox_vertex_shader.glsl", "src/shaders/skybox/skybox_fragment_shader.glsl", skybox_shader)) {
        std::cerr << "Failed to load skybox shaders\n";
        return -1;
    }
    auto camera_object = main_scene.create_game_object("Camera")
        .with_camera()
            .with_background(glm::vec4(1.0, 1.0, 0, 1.0))
        .with_transform(glm::vec3(0.0, 2.0, 10.0))
        .with_script(std::make_shared<CameraMovementScript>())
        .build();

    auto camera_transform = camera_object->get_component<TransformComponent>();
    auto camera_component = camera_object->get_component<CameraComponent>();

        
    // Set light properties
    LightProperties light_properties;
    light_properties.direction = glm::vec3(1.2f, -15.0f, 2.0f);
    light_properties.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light_properties.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light_properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    double current_time = 0.0;
    double previous_time = 0.0;
    double delta_time = 0.0;

    int frame_count = 0;
    double fps_time = 0.0;

    main_scene.start();

    while (!glfwWindowShouldClose(window)) {
        current_time = glfwGetTime();
        delta_time = current_time - previous_time;
        previous_time = current_time;

        InputState::delta_time = delta_time;

        glm::mat4 view = camera_component->get_view_matrix(camera_transform->position, camera_transform->get_front(), camera_transform->get_up());
        glm::mat4 projection = camera_component->get_projection_matrix(static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT);


        camera_component->clear(view, projection);

        render_scene(main_scene, projection, view, camera_transform->position, light_properties);

        main_scene.update();

        InputState::update_previous_key_state();
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (debug_mode) {
            frame_count++;
            fps_time += delta_time;
            if (fps_time >= 1.0) {
                std::cout << "FPS: " << frame_count << std::endl;
                frame_count = 0;
                fps_time = 0.0;
            }
        }

        double frame_end_time = glfwGetTime();
        double frame_time = frame_end_time - current_time;
        if (frame_time < FRAME_DURATION) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_DURATION - frame_time));
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}