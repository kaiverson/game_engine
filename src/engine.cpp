#include "engine.hpp"

// engine.cpp
bool EngineCore::initialize() {
    if (!create_window()) return false;
    if (!init_gl_context()) return false;
    if (!setup_callbacks()) return false;
    if (!setup_scene()) return false;

    IMGUI_CHECKVERSION();

    return true;
}

bool EngineCore::run() {
    if (!window) return false;

    main_loop();
    return true;
}

void EngineCore::shutdown() {
    // active_scene->reset(); 

    if (window) {
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

        // Rendering
        const float aspect_ratio = static_cast<float>(config.screen_width) / config.screen_height;
        render_scene(*active_scene, aspect_ratio);

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
            .with_background(glm::vec4(0.5, 1.0, 0, 1.0))
        .with_transform(TransformParams{.position = glm::vec3(0, 2, 10)})
        .with_script(std::make_shared<CameraMovementScript>())
        .build();

    active_scene->set_main_camera(camera);

    // Load assets
    AssetManager assets;
    auto material = assets.create_material()
        .with_preset(MaterialPreset::URP)
        .build();

    // Create ground entity
    auto ground_texture = std::make_shared<Texture>("src/textures/ground.jpg");
    active_scene->create_game_object("Ground")
        .with_render_mesh("src/objects/plane_usd.obj")
        .with_material(material)
        .with_transform(TransformParams{.scale = glm::vec3(3.0f)})
        .build();

    active_scene->create_game_object("src/objects/UTAH_BLEND.obj")
        .with_render_mesh("src/objects/UTAH_BLEND.obj")
        .with_material(material)
        .with_transform(TransformParams{.position = glm::vec3(0.0, 3.0, 0.0)})
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
