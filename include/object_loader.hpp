#pragma once
#include <iostream>
#include <vector>
#include <cstddef>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>

struct Face {
    int vertex_index[3] = {0, 0, 0};
    int texture_index[3] = {0, 0, 0};
    int normal_index[3] = {0, 0, 0};
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