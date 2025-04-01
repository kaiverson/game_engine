#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream> 
#include <sstream>
#include <iostream>

class Shader {
private:
    GLuint program = 0;
    
    bool load(const std::string &vertex_path, const std::string &fragment_path);

public:
    Shader(const std::string &directory);
    Shader(const std::string &vertex_path, const std::string &fragment_path);
    GLuint get_program() const  { return program; };
    void use() const;
};

#endif // SHADER_HPP