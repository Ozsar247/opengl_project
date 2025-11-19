#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<unsigned int> Shader::allShaders;
Shader* Shader::currentShader = nullptr;

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. Get File Data
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open Files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        // Put Buffers into Stream
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Close Files
        vShaderFile.close();
        fShaderFile.close();

        // Retrive Code
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure err) {
        std::cout << "ERROR: Shader file read fail!" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile Shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex Shader Compilation
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Error Handling
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex Shader compilation error!\n" << infoLog << std::endl;
    }

    // Fragment Shader Compilation
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // Error Handling
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex Shader compilation error!\n" << infoLog << std::endl;
    }

    // Create Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Error Handling
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR: Shader program linking error!\n" << infoLog << std::endl;
    }

    // Delete Shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    allShaders.push_back(ID);
}

