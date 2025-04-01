#include "shader.hpp"

Shader::Shader(const std::string &directory) {
    const std::string vertex_path = directory + "/vertex.glsl";
    const std::string fragment_path = directory + "/fragment.glsl";

    bool success = load(vertex_path, fragment_path);
    if (!success) {
        std::cerr << "Shader failed to initialize\n";
    }
}

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path) {
    bool success = load(vertex_path, fragment_path);
    
    if (!success) {
        std::cerr << "Shader failed to initialize\n";
    }
}

bool Shader::load(const std::string &vertex_path, const std::string &fragment_path) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    if (!vertex_file.is_open()) {
        std::cerr << "Failed to load vertex shader " << vertex_path << "\n";
        return false;
    }

    if (!fragment_file.is_open()) {
        std::cerr << "Failed to load fragment shader " << fragment_path << "\n";
        return false;
    }

    std::stringstream vertex_stream, fragment_stream;
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    std::string vertex_code = vertex_stream.str();
    std::string fragment_code = fragment_stream.str();

    const char *vertex_shader_code = vertex_code.c_str();
    const char *fragment_shader_code = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATIO_FAILED\n" << info_log << "\n";
        return false;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATIO_FAILED\n" << info_log << "\n";
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << "\n";
        return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return true;
}

void Shader::use() const { 
    glUseProgram(program); 
}


