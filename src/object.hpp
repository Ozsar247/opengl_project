#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>

class Object {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    Shader* shader = nullptr;

    virtual void drawInspector() {
        ImGui::Text("Base Object");

        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
        ImGui::DragFloat3("Scale",     glm::value_ptr(scale),     0.1f);
    }

    virtual ~Object() = default;
    virtual void update(float dt) = 0;
    virtual void render(glm::mat4 view, glm::mat4 projection, Shader* defaultShader) = 0;
    
};