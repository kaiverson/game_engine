#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "game_object.hpp"
#include "components/transform_component.hpp"
#include "components/render_mesh_component.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

Ray screen_to_world_ray(int mouse_x, int mouse_y, int screen_width, int screen_height, const glm::mat4 &view, const glm::mat4 &projection);

std::shared_ptr<GameObject> raycast(const Ray &ray, const std::vector<std::shared_ptr<GameObject>> &game_objects);