#ifndef COMMONHEADERS_H
#define COMMONHEADerS_H

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

#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine_config.hpp"
#include "utils.hpp"
#include "callbacks.hpp"
#include "game_object.hpp"
#include "game_object_builder.hpp"
#include "material.hpp"
#include "material_builder.hpp"
#include "bounding_box.hpp"
#include "asset_manager.hpp"
#include "components/transform_component.hpp"
#include "components/render_mesh_component.hpp"
#include "components/script_component.hpp"
#include "components/camera_component.hpp"
#include "input_state.hpp"
#include "scene.hpp"
#include "skybox.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include "change_color_script.hpp"
#include "camera_movement_script.hpp"


#endif // COMMON_HEADERS_H