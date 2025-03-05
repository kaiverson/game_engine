#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

std::vector<float> generate_normal_lines(const std::vector<float> &vertices, const std::vector<float> &normals, float length = 0.1f);
std::string read_file(const std::string &file_path);

#endif // UTILS_HPP