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
#include "shader_utils.hpp"
#include "game_object.hpp"
#include "scene.hpp"
#include "components/transform_component.hpp"
#include "components/material_component.hpp"
#include "components/render_mesh_component.hpp"

bool load_shader(const std::string& vertex_path, const std::string& fragment_path, GLuint& shader_program);
void render_scene(const Scene scene, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& camera_position, const LightProperties& light_properties);
void draw_normals(const std::shared_ptr<RenderMeshComponent>& render_mesh, const glm::mat4& model, const glm::mat4& projection, const glm::mat4& view);
GLuint create_normal_shader_program();

#endif // RENDER_HPP