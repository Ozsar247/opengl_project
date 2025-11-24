#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>

class Object {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    virtual ~Object() {}
    virtual void update(float dt) = 0;
    virtual void render(glm::mat4 view, glm::mat4 projection, Shader* shader) = 0;
    
};