#include "engine.hpp"

// engine.cpp
bool EngineCore::initialize() {
    try {
        if (!create_window()) throw std::runtime_error("Window creation failed");
        if (!init_gl_context()) throw std::runtime_error("GL context initialization failed");
        if (!setup_callbacks()) throw std::runtime_error("Callback setup failed");
        if (!setup_scene()) throw std::runtime_error("Scene setup failed");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
            throw std::runtime_error("ImGui GLFW implementation failed");
        }
        if (!ImGui_ImplOpenGL3_Init("#version 330")) {
            throw std::runtime_error("ImGui OpenGL3 implementation failed");
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Engine initialization failed: " << e.what() << "\n";
        shutdown();
        return false;
    }
}

bool EngineCore::run() {
    if (!window) return false;

    main_loop();
    return true;
}

void EngineCore::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwSetWindowUserPointer(window, nullptr);
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void EngineCore::main_loop() {
    double last_time = glfwGetTime();
    double accumulator = 0.0;
    const double time_step = 1.0 / config.target_fps;

    while (!glfwWindowShouldClose(window)) {
        const double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Fixed timestep update
        accumulator += delta_time;
        while (accumulator >= time_step) {
            active_scene->update(static_cast<float>(time_step));
            InputState::update_previous_key_state();
            accumulator -= time_step;
        }

        /* IMGUI */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        render_ui();
        /* END IMGUI*/

        // Rendering
        const float aspect_ratio = static_cast<float>(config.screen_width) / config.screen_height;
        render_scene(aspect_ratio);

        /* IMGUI */
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* END IMGUI */

        glfwSwapBuffers(window);
        glfwPollEvents();

        process_debug_input();
    }
}



bool EngineCore::create_window() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(
        config.screen_width,
        config.screen_height,
        "Game Engine",
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    input_state.set_window(window);

    return true;
}

bool EngineCore::init_gl_context() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glViewport(0, 0, config.screen_width, config.screen_height);
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool EngineCore::setup_callbacks() {
    if (!window) return false;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetErrorCallback(error_callback);

    return true;
}

bool EngineCore::setup_scene() {
    // Scene initialization
    active_scene = std::make_unique<Scene>();
    InputState::set_active_scene(active_scene.get());
    
    // Create camera entity
    auto camera = active_scene->create_game_object("MainCamera")
        .with_camera()
            .with_background(glm::vec4(0.67, 0.79, 0.92, 1.0))
        .with_transform(TransformParams{.position = glm::vec3(0, 2, 10)})
        .with_script(std::make_shared<CameraMovementScript>())
        .build();

    active_scene->set_main_camera(camera);

    // Load assets
    AssetManager assets;
    auto material = assets.create_material()
        .with_preset(MaterialPreset::Simple)
        .build();

    auto cube_material = assets.create_material()
        .with_preset(MaterialPreset::URP)
        .build();

    auto cottage_material = assets.create_material()
        .with_preset(MaterialPreset::URP)
        .build();



    auto ground_texture = std::make_shared<Texture>("src/textures/ground.jpg");
    active_scene->create_game_object("Ground")
        .with_render_mesh("src/objects/plane_usd.obj")
        .with_material(material)
        .with_transform(TransformParams{.scale = glm::vec3(100.0f)})
        .build();

    active_scene->create_game_object("UtahBlendModel")
        .with_render_mesh("src/objects/UTAH_BLEND.obj")
        .with_material(material)
        .with_transform(TransformParams{.position = glm::vec3(0.0, 1.0, 0.0)})
        .build();

    active_scene->create_game_object("Cube")
        .with_render_mesh("src/objects/cube.obj")
        .with_material(cube_material)
        .with_transform(TransformParams{.position = glm::vec3(2.0, 0.5, 2.0)})
        .build();

    active_scene->create_game_object("Cottage")
        .with_render_mesh("src/objects/Cottage.fbx")
        .with_material(cottage_material)
        .with_transform(TransformParams{.position = glm::vec3(-20, 0, -20), .scale = glm::vec3(0.01)})
        .build();

    active_scene->start();

    return true;
}

void EngineCore::process_debug_input() {
    if (!config.debug_mode) return;

    if (InputState::is_key_just_pressed(GLFW_KEY_F1)) {
        config.wireframe_mode = !config.wireframe_mode;
        // set_wireframe_mode(config.wireframe_mode);
    }

    if (InputState::is_key_just_pressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

bool EngineCore::render_scene(const float aspect_ratio) {
    bool success = true;
    
    auto camera = active_scene->get_main_camera();
    if (!camera) {
        std::cerr << "Render: No main camera\n";
        return false;
    }

    LightProperties light_properties;
    light_properties.direction = glm::vec3(1.2f, -15.0f, 2.0f);
    light_properties.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light_properties.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light_properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    auto camera_transform = active_scene->get_main_camera()->get_component<TransformComponent>();
    auto camera_position = camera_transform->position;

    auto camera_component = active_scene->get_main_camera_component();
    glm::mat4 projection = camera_component->get_projection_matrix(aspect_ratio);
    glm::mat4 view = camera_component->get_view_matrix(camera_transform->position, camera_transform->get_front(), camera_transform->get_up());

    camera_component->set_viewport();
    camera_component->clear(view, projection);

    for (auto &game_object : active_scene->get_game_objects()) {
        auto render_mesh_component = game_object->get_component<RenderMeshComponent>();
        if (!render_mesh_component) {
            continue;
        }

        success = render_mesh_component->render(projection, view, camera_position, light_properties);
        if (!success) {
            std::cerr << "Render: '" << game_object->name << "' failed to render\n";
        }
    }

    return true;
}


bool EngineCore::render_ui() {
    ImGui::Begin("Engine Debug Menu");

    // Display FPS
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    // Scene Hierarchy
    if (ImGui::CollapsingHeader("Scene Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (active_scene) {
            for (const auto& game_object : active_scene->get_game_objects()) {
                
                if (ImGui::Selectable(game_object->name.c_str(),
                                      selected_game_object == game_object.get())) {
                    selected_game_object = game_object.get();
                }
            }
        } else {
            ImGui::TextDisabled("No active scene");
        }
    }

    // Debug controls
    if (ImGui::CollapsingHeader("Debug Controls")) {
        if (ImGui::Checkbox("Wireframe Mode", &config.wireframe_mode)) {
            // set_wireframe_mode(config.wireframe_mode);
        }
        ImGui::Checkbox("Debug Mode", &config.debug_mode);
    }

    if (ImGui::Button("Exit")) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    ImGui::End();

    if (selected_game_object) {
        draw_properties_window();
    }

    return true;
}

void EngineCore::draw_properties_window() {
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    static char name_buffer[128];
    strncpy_s(name_buffer, selected_game_object->name.c_str(), sizeof(name_buffer));
    name_buffer[sizeof(name_buffer) - 1] = '\0';

    if (ImGui::InputText("Name", name_buffer, sizeof(name_buffer))) {
        selected_game_object->name = std::string(name_buffer);
    }

    ImGui::Separator();

    selected_game_object->draw_inspector_ui();

    ImGui::End();
}