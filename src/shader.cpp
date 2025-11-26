#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<unsigned int> Shader::allShaders;
Shader* Shader::currentShader = nullptr;

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geomPath) {
    // 1. Get File Data
    std::string vertexCode;
    std::string fragmentCode;
    std::string geomCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open Files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        if (geomPath) {
            gShaderFile.open(geomPath);
        }
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        
        // Put Buffers into Stream
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (geomPath) {
            gShaderStream << gShaderFile.rdbuf();
        }

        // Close Files
        vShaderFile.close();
        fShaderFile.close();
        if (geomPath) {
            gShaderFile.close();
        }

        // Retrive Code
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (geomPath) {
            geomCode = gShaderStream.str();
        }
    } catch(std::ifstream::failure err) {
        std::cout << "ERROR: Shader file read fail!" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = nullptr;
    if (geomPath) {
        gShaderCode = geomCode.c_str();
    } 

    // 2. Compile Shaders
    unsigned int vertex, fragment, geometry;
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

    if (gShaderCode) {
        // Geometry Shader Compilation
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);

        // Error Handling
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout << "ERROR: Geometry Shader compilation error!\n" << infoLog << std::endl;
        }
    }

    // Create Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (gShaderCode) {
        glAttachShader(ID, geometry);
    }
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
    if (geometry) {
        glDeleteShader(geometry);
    }

    allShaders.push_back(ID);
}

