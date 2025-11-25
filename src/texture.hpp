#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <stb/stb_image.h>
#include "shader.hpp"

class Texture {
public:
    unsigned int texture = 0;   // GL handle
    std::string path;
    int id = 0;                 // unique engine ID

    Texture() {
        id = nextId++;
        registry.push_back(this);
        registryMap[id] = this;
    }

    Texture(const char* filePath, bool flip = true) : Texture() {
        load(filePath, flip);
    }

    ~Texture() {
        cleanup();

        registry.erase(std::remove(registry.begin(), registry.end(), this), registry.end());
        registryMap.erase(id);
    }

    void load(const char* filePath, bool flip = true) {
        path = filePath;

        if (texture != 0)
            glDeleteTextures(1, &texture);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(flip ? 1 : 0);

        int width, height, channels;
        unsigned char* data = stbi_load(filePath, &width, &height, &channels, 0);

        if (data) {
            GLenum format = (channels == 1 ? GL_RED :
                             channels == 3 ? GL_RGB :
                                             GL_RGBA);

            glTexImage2D(GL_TEXTURE_2D, 0, format,
                         width, height, 0,
                         format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Failed to load texture: " << filePath << std::endl;
        }

        stbi_image_free(data);

        // ALWAYS update registryMap entry
        registryMap[id] = this;
    }

    void bind(unsigned int slot, const std::string& uniformName) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);

        Shader* curr = Shader::getCurrentShader();
        if (curr) curr->setInt(uniformName.c_str(), slot);
    }

    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void cleanup() {
        if (texture != 0) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
    }

    static const std::vector<Texture*>& getAllTextures() {
        return registry;
    }

    static Texture* getById(int id) {
        auto it = registryMap.find(id);
        return it != registryMap.end() ? it->second : nullptr;
    }

private:
    static inline int nextId = 1;
    static inline std::vector<Texture*> registry;
    static inline std::unordered_map<int, Texture*> registryMap;
};
