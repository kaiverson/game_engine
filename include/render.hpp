#ifndef RENDER_HPP
#define RENDER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include "structs.hpp"
#include "game_object.hpp"
#include "scene.hpp"
#include "components/transform_component.hpp"
#include "components/render_mesh_component.hpp"

void render_scene(const Scene& scene, const float aspect_ratio);

#endif // RENDER_HPP