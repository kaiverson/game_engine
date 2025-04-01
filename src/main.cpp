#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine.hpp"

bool debug_mode = true;
bool wireframe_mode = true;

int main() {
    EngineCore engine;
    if (!engine.initialize()) return EXIT_FAILURE;
    if (!engine.run()) return EXIT_FAILURE;
    engine.shutdown();
    return EXIT_SUCCESS;
}

// bool wireframe_mode = false;
// bool debug_mode = false;

// GLuint SCREEN_WIDTH = 1000;
// GLuint SCREEN_HEIGHT = 800;

// const int TARGET_FPS = 120;
// const double FRAME_DURATION = 1.0 / TARGET_FPS;

// std::shared_ptr<GameObject> camera_object;

// int main() {
//     EngineCore engine;
//     if (!engine.initialize()) return EXIT_FAILURE;
//     if (!engine.run()) return EXIT_FAILURE;
//     engine.shutdown();
//     return EXIT_SUCCESS;
// }


// int main() {
//     GLFWwindow *window;
//     bool success = set_up_window(window);

//     // Create Scene
//     Scene main_scene;
//     InputState::set_active_scene(&main_scene);

//     std::shared_ptr<Texture> ground_base_texture = std::make_shared<Texture>("src/textures/ground.jpg");

//     AssetManager asset_manager;
//     auto material = asset_manager.create_material()
//                         .with_preset(MaterialPreset::URP)
//                         // .with_uniform("material.base_map", ground_base_texture)
//                         // .with_uniform("material.normal_map", ground_base_texture)
//                         // .with_uniform("material.metallic_map", ground_base_texture)
//                         .build();

//     auto g_mat = asset_manager.create_material()
//                         .build();

//     auto g_shader = std::make_shared<Shader>("src/shaders/quad");
//     g_mat->set_shader(g_shader);
//     g_mat->set_uniform("texture1", ground_base_texture);

//     // Make asset manager
//     // add create_material method
//     // create the MaterialBuilder
//     // Convert stuff to the material builder.
//     // maybe also add create_shader method

//     auto ground = main_scene.create_game_object("Ground")
//         .with_render_mesh("src/objects/plane_usd.obj")
//             .with_material(g_mat)
//         .with_transform(TransformParams{.scale = glm::vec3(3.0f)})
//         .build();

//     auto tex = main_scene.create_game_object("Teapot")
//         .with_render_mesh("src/objects/UTAH_BLEND.obj")
//             .with_material(material)
//         .with_script(std::make_shared<ChangeColorScript>())
//         .build();  

//     auto camera_object = main_scene.create_game_object("Camera")
//         .with_camera()
//             .with_background(glm::vec4(1.0, 1.0, 0, 1.0))
//             // .with_skybox("src/textures/skybox", skybox_shader)
//         .with_transform(TransformParams{.position = glm::vec3(0, 2, 10)})
//         .with_script(std::make_shared<CameraMovementScript>())
//         .build();

//     auto camera_transform = camera_object->get_component<TransformComponent>();
//     auto camera_component = camera_object->get_component<CameraComponent>();
        
//     // Set light properties
//     LightProperties light_properties;
//     light_properties.direction = glm::vec3(1.2f, -15.0f, 2.0f);
//     light_properties.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
//     light_properties.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
//     light_properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);

//     float current_time = 0.0;
//     float previous_time = 0.0;
//     float delta_time = 0.0;

//     int frame_count = 0;
//     float fps_time = 0.0;

//     main_scene.start();

//     while (!glfwWindowShouldClose(window)) {
//         current_time = glfwGetTime();
//         delta_time = current_time - previous_time;
//         previous_time = current_time;

//         InputState::delta_time = delta_time;

//         glm::mat4 view = camera_component->get_view_matrix(camera_transform->position, camera_transform->get_front(), camera_transform->get_up());
//         glm::mat4 projection = camera_component->get_projection_matrix(static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT);

//         camera_component->set_viewport();
//         camera_component->clear(view, projection);
//         render_scene(main_scene, projection, view, camera_transform->position, light_properties);

//         main_scene.update();


//         InputState::update_previous_key_state();
//         glfwSwapBuffers(window);
//         glfwPollEvents();

//         if (debug_mode) {
//             frame_count++;
//             fps_time += delta_time;
//             if (fps_time >= 1.0) {
//                 std::cout << "FPS: " << frame_count << std::endl;
//                 frame_count = 0;
//                 fps_time = 0.0;
//             }
//         }

//         if (InputState::is_key_just_pressed(GLFW_KEY_L)) material->list_all_uniforms();
//         if (InputState::is_key_just_pressed(GLFW_KEY_C)) material->check_uniforms();
//         if (InputState::is_key_just_pressed(GLFW_KEY_T)) camera_transform->print_transform();

//         double frame_end_time = glfwGetTime();
//         double frame_time = frame_end_time - current_time;
//         if (frame_time < FRAME_DURATION) {
//             std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_DURATION - frame_time));
//         }
//     }

//     glfwDestroyWindow(window);
//     glfwTerminate();
//     return 0;
// }




// bool set_up_window(GLFWwindow *&window) {
//     if (!glfwInit()) {
//         std::cerr << "Failed to initialize GLFW\n";
//         return false;
//     }

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
//     window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GAME", nullptr, nullptr);
//     if (!window) {
//         std::cerr << "Failed to create GLFW window\n";
//         glfwTerminate();
//         return false;
//     }

//     glfwMakeContextCurrent(window);
//     InputState::set_window(window);

//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//         std::cerr << "Failed to initialize GLAD\n";
//         return false;
//     }

//     glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//     glEnable(GL_DEPTH_TEST);

//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     glfwSetCursorPosCallback(window, mouse_callback);
//     glfwSetKeyCallback(window, key_callback);
//     glfwSetMouseButtonCallback(window, mouse_button_callback);

//     return true;
// }