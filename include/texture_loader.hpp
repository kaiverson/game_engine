#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <string>

GLuint load_texture(const std::string& path);
GLuint load_cubemap(const std::vector<std::string>& faces);

#endif // TEXTURE_LOADER_HPP