#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include "shader.hpp"

class Framebuffer {
public:
    unsigned int id; // your own logical ID

    // Static registry
    inline static std::unordered_map<unsigned int, Framebuffer*> registry;

    Framebuffer(unsigned int id, double SCR_W, double SCR_H, Shader* shader)
        : id(id), width(SCR_W), height(SCR_H)
    {
        // Register this framebuffer
        registry[id] = this;

        // Setup screen shader uniform
        shader->use();
        shader->setInt("screenTexture", 0);

        // Generate framebuffer
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Generate texture
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     (int)width, (int)height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, textureColorbuffer, 0);

        // Renderbuffer
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                              (int)width, (int)height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ResizeFrameBuffer(int w, int h) {
        width = w;
        height = h;
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    }

    void BindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
    }

    void UnbindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int Texture() {
        return textureColorbuffer;
    }

    void CleanupFrameBuffer() {
        // Cleanup GL objects
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorbuffer);
        glDeleteRenderbuffers(1, &rbo);

        // Remove from registry
        registry.erase(id);
    }

private:
    double width;
    double height;

    unsigned int framebuffer = 0;
    unsigned int textureColorbuffer = 0;
    unsigned int rbo = 0;
};
