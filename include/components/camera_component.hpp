#pragma once

#include "component.hpp"
#include "input_state.hpp"
#include "skybox.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class CameraComponent : public Component {
public:
    enum class ClearFlags {
        Skybox,
        SolidColor,
        DepthOnly,
        Nothing
    };

    enum class Projection {
        Perspective,
        Orthographic
    };

    enum class FOVAxis {
        Vertical,
        Horizontal
    };

    ClearFlags clear_flags;
    Skybox skybox;
    GLuint skybox_texture;
    GLuint skybox_shader;
    glm::vec4 background; // RGBA
    uint32_t culling_mask;
    Projection projection;
    FOVAxis fov_axis;
    float field_of_view;
    glm::vec2 clipping_planes; // x = near, y = far
    glm::vec4 viewport_rect; // x, y, width, height

    CameraComponent(ClearFlags clear_flags = ClearFlags::Skybox,
                    Skybox skybox = Skybox(),
                    glm::vec4 background = glm::vec4(0.2f, 0.3f, 1.0f, 1.0f),
                    uint32_t culling_mask = 0xFFFFFFFF,
                    Projection projection = Projection::Perspective,
                    FOVAxis fov_axis = FOVAxis::Vertical,
                    float field_of_view = 45.0f,
                    glm::vec2 clipping_planes = glm::vec2(0.1f, 100.0f),
                    glm::vec4 viewport_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
        : clear_flags(clear_flags), skybox(skybox), background(background), 
          culling_mask(culling_mask), projection(projection), fov_axis(fov_axis), 
          field_of_view(field_of_view), clipping_planes(clipping_planes), 
          viewport_rect(viewport_rect) {}

    glm::mat4 get_view_matrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 get_projection_matrix(float aspect_ratio) const {
        if (projection == Projection::Perspective) {
            return glm::perspective(glm::radians(field_of_view), aspect_ratio, clipping_planes.x, clipping_planes.y);
        } else {
            float ortho_size = field_of_view; // Assuming field_of_view is used as orthographic size
            float left = -ortho_size * aspect_ratio;
            float right = ortho_size * aspect_ratio;
            float bottom = -ortho_size;
            float top = ortho_size;
            return glm::ortho(left, right, bottom, top, clipping_planes.x, clipping_planes.y);
        }
    }

    void clear(const glm::mat4 &view, const glm::mat4 &projection) {
        switch (clear_flags) {
            case ClearFlags::Skybox:
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (!skybox.render(view, projection)) {
                    clear_flags = ClearFlags::SolidColor;
                    std::cerr << "[Error] Skybox is not loaded! Switching to Solid Color mode\n";
                }
                break;
            case ClearFlags::SolidColor:
                glClearColor(background.r, background.g, background.b, background.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case ClearFlags::DepthOnly:
                glClear(GL_DEPTH_BUFFER_BIT);
                break;
            case ClearFlags::Nothing:
                // Do nothing
                break;
        }
    }

    void update(GameObject& gameObject) override {
        if (InputState::is_key_pressed(GLFW_KEY_G)) {
            background = glm::vec4(0.5, 1.0, 0.6, 1.0);
        } else {
            background = glm::vec4(0.5, 0.6, 1.0, 1.0);
        }
    }
};