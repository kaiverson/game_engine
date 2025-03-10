#pragma once
#include <iostream>
#include <vector>
#include <cstddef>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <array>

struct Face {
    std::array<int, 3> vertex_index = {0, 0, 0};
    std::array<int, 3> texture_index = {0, 0, 0};
    std::array<int, 3> normal_index = {0, 0, 0};
};

struct ObjModel {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<Face> faces;
};

bool load_obj(const std::string &filename, ObjModel &model);