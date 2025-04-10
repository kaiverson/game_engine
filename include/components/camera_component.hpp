#pragma once

#include "component.hpp"
#include "input_state.hpp"
#include "skybox.hpp"
#include "engine_config.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <array>

extern EngineConfig config;

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

    glm::mat4 get_projection_matrix(const float aspect_ratio) const {
        float fov = field_of_view;
        if (fov_axis == FOVAxis::Horizontal) {
            fov = glm::degrees(2.0f * atan(tan(glm::radians(field_of_view) / 2.0f) * aspect_ratio));
        }

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

    void set_viewport() const {
        glViewport(static_cast<GLint>(viewport_rect.x * config.screen_width), 
                   static_cast<GLint>(viewport_rect.y * config.screen_width),
                   static_cast<GLint>(viewport_rect.z * config.screen_width), 
                   static_cast<GLint>(viewport_rect.w * config.screen_width));
    }

    void update(GameObject& game_object, const float delta_time) override {
        
    }

    void draw_inspector_ui() override {
        ImGui::Text("Camera Component");

        const char* clear_flags_items[] = {"Skybox", "SolidColor", "DepthOnly", "Nothing"};
        int clear_flags_index = static_cast<int>(clear_flags);
        if (ImGui::Combo("Clear Flags", &clear_flags_index, clear_flags_items, IM_ARRAYSIZE(clear_flags_items))) {
            clear_flags = static_cast<ClearFlags>(clear_flags_index);
        }

        ImGui::Text("Background Color");
        float bg_color[4] = { background.r, background.g, background.b, background.a };
        if (ImGui::ColorEdit4("##Background Color", bg_color)) {
            background = glm::vec4(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
        }

        ImGui::Text("Culling Mask");
        ImGui::InputScalar("Culling Mask", ImGuiDataType_U32, &culling_mask);

        const char* projection_items[] = { "Perspective", "Orthographic" };
        int projection_index = static_cast<int>(projection);
        if (ImGui::Combo("Projection", &projection_index, projection_items, IM_ARRAYSIZE(projection_items))) {
            projection = static_cast<Projection>(projection_index);
        }

        const char* fov_axis_items[] = { "Vertical", "Horizontal" };
        int fov_axis_index = static_cast<int>(fov_axis);
        if (ImGui::Combo("FOV Axis", &fov_axis_index, fov_axis_items, IM_ARRAYSIZE(fov_axis_items))) {
            projection = static_cast<Projection>(projection_index);
        }

        ImGui::Text("Field of View");
        ImGui::DragFloat("##Field of View", &field_of_view, 0.1f, 1.0f, 180.0f);

        ImGui::Text("Clipping Planes");
        float clipping_planes_array[2] = { clipping_planes.x, clipping_planes.y };
        if (ImGui::DragFloat2("##Clipping Planes", clipping_planes_array, 0.1f, 0.01f, 1000.0f)) {
            clipping_planes = glm::vec2(clipping_planes_array[0], clipping_planes_array[1]);
        }

        ImGui::Text("Viewport Rect");
        float viewport_rect_array[4] = { viewport_rect.x, viewport_rect.y, viewport_rect.z, viewport_rect.w };
        if (ImGui::DragFloat4("##Viewport Rect", viewport_rect_array, 0.01f, 0.0f, 1.0f)) {
            viewport_rect = glm::vec4(viewport_rect_array[0], viewport_rect_array[1], viewport_rect_array[2], viewport_rect_array[3]);
        }
    }
};