#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "object_loader.hpp"
// #include "texture_loader.hpp"
#include "utils.hpp"
#include "shader_utils.hpp"
#include "callbacks.hpp"
#include "render.hpp"
#include "game_object.hpp"
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
bool move_forward = false;
bool move_backward = false;
bool move_left = false;
bool move_right = false;
bool move_up = false;
bool move_down = false;

GLuint SCREEN_WIDTH = 1000;
GLuint SCREEN_HEIGHT = 800;

std::shared_ptr<GameObject> camera_object;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    
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

    auto game_object = main_scene.create_game_object("Block");
    ObjModel obj_model;
    if (!load_obj("src/objects/plane.obj", obj_model)) {
        std::cerr << "FAILED TO LOAD OBJECT\n";
    }
    MaterialProperties material_properties;
    material_properties.smoothness = 0.5f;
    auto materialComponent = std::make_shared<MaterialComponent>(material_properties);
    materialComponent->set_texture(materialComponent->material.base_map, "src/objects/dragon/Material_baseColor.png");
    // materialComponent->set_texture(materialComponent->material.metallic_map, "src/objects/dragon/Material_baseColor.png");
    materialComponent->set_texture(materialComponent->material.normal_map, "src/objects/dragon/Material_normal.png");
    // materialComponent->set_texture(materialComponent->material.occlusion_map, "src/objects/dragon/Material_baseColor.png");
    game_object->add_component(materialComponent);
    auto renderMeshComponent = std::make_shared<RenderMeshComponent>(obj_model);
    game_object->add_component(renderMeshComponent);
    game_object->get_component<TransformComponent>()->scale = glm::vec3(3.0, 3.0, 3.0);
    game_object->get_component<TransformComponent>()->rotation = glm::quat(glm::vec3(-1.57, 0, 0));
    auto changeColorScript = std::make_shared<ChangeColorScript>();
    game_object->add_component(changeColorScript);

    // Create Camera GameObject
    std::vector<std::string> skybox_faces = {
        "src/objects/dragon/stone_bricks.png",
        "src/objects/dragon/stone_bricks.png",
        "src/objects/dragon/stone_bricks.png",
        "src/objects/dragon/stone_bricks.png",
        "src/objects/dragon/stone_bricks.png",
        "src/objects/dragon/stone_bricks.png"
    };

    GLuint skybox_shader;
    if (!load_shader("src/shaders/skybox/skybox_vertex_shader.glsl", "src/shaders/skybox/skybox_fragment_shader.glsl", skybox_shader)) {
        std::cerr << "Failed to load skybox shaders\n";
        return -1;
    }
    Skybox skybox = Skybox(skybox_faces, skybox_shader);

    camera_object = main_scene.create_game_object("Camera");
    auto camera_component = std::make_shared<CameraComponent>(CameraComponent::ClearFlags::SolidColor, skybox);
    auto camera_transform = camera_object->get_component<TransformComponent>();
    camera_transform->position = glm::vec3(0.0f, 2.0f, 10.0f);
    camera_object->add_component(camera_component);
    camera_object->add_component(std::make_shared<CameraMovementScript>());

    // Load shaders
    GLuint shaderProgram;
    if (!load_shader("src/shaders/utah/vertex_shader.glsl", "src/shaders/utah/fragment_shader.glsl", shaderProgram)) {
        std::cerr << "Failed to load shaders\n";
        return -1;
    }
    

    ShaderUniformLocations uniform_locations = get_uniform_locations(shaderProgram);

    // Set light properties
    LightProperties light_properties;
    light_properties.direction = glm::vec3(1.2f, -15.0f, 2.0f);
    light_properties.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light_properties.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light_properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    RenderData render_data;
    render_data.shader_program = shaderProgram;
    render_data.game_objects.push_back(game_object);
    render_data.game_objects.push_back(camera_object);

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

        render_scene(render_data, uniform_locations, projection, view, camera_transform->position, light_properties);

        main_scene.update();

        if (debug_mode) {
            frame_count++;
            fps_time += delta_time;
            if (fps_time >= 1.0) {
                std::cout << "FPS: " << frame_count << std::endl;
                frame_count = 0;
                fps_time = 0.0;
            }
        }

        InputState::update_previous_key_state();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}