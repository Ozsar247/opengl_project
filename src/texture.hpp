#pragma once

#include <glad/glad.h>
#include <iostream>

#include "shader.hpp"

class Texture {
public:
    unsigned int texture = 0;

    Texture(const char* path, bool flip = true);

    void bind(unsigned int slot, const std::string& uniformName) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Ensure shader is active before setting
        Shader::getCurrentShader()->setInt(uniformName.c_str(), slot);
    }

    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }


    void cleanup() {
        glDeleteTextures(1, &texture);
    }
private:
    unsigned int textures = 0;
};
