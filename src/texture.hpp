#pragma once

#include <glad/glad.h>
#include <iostream>

#include "shader.hpp"

class Texture {
public:
    unsigned int texture = 0;

    Texture(const char* path, bool flip = true);

    void bind(unsigned int slot = -1, char* name = (char*)"texture") {

        if (slot == -1) {
            glActiveTexture(GL_TEXTURE0 + textures);
            glBindTexture(GL_TEXTURE_2D, texture);
        } else {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture);
        }

        if (name == (char*)"texture") {
            Shader::getCurrentShader()->setInt((char*)"texture" + std::to_string(slot), slot);
        } else {
            Shader::getCurrentShader()->setInt((char*)name, slot);
        }
        textures += 1;
    }

    void cleanup() {
        glDeleteTextures(1, &texture);
        textures -= 1;
    }
private:
    unsigned int textures = 0;
};
