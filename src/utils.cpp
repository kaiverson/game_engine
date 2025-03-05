#include "utils.hpp"
#include <glm/glm.hpp>

std::vector<float> generate_normal_lines(const std::vector<float>& vertices, const std::vector<float>& normals, float length) {
    std::vector<float> normal_lines;
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 normal(normals[i], normals[i + 1], normals[i + 2]);
        glm::vec3 end = vertex + normal * length;

        normal_lines.push_back(vertex.x);
        normal_lines.push_back(vertex.y);
        normal_lines.push_back(vertex.z);

        normal_lines.push_back(end.x);
        normal_lines.push_back(end.y);
        normal_lines.push_back(end.z);
    }
    return normal_lines;
}

std::string read_file(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}