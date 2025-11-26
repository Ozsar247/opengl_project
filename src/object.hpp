#pragma once

#include "shader.hpp"
#include "imgui/imgui.h"

#include <glm/glm.hpp>

#ifndef MAX_BONE_INFLUENCE
#define MAX_BONE_INFLUENCE 4
#endif

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Object {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    Shader* shader = nullptr;

    std::vector<unsigned int*> GetVAOList() {return VAO;}
    std::vector<unsigned int*> GetVBOList() {return VBO;}
    std::vector<unsigned int*> GetEBOList() {if (EBO.size() > 0) return EBO;}

    std::vector<std::vector<Vertex>> obj_vertices;
    std::vector<std::vector<unsigned int>> obj_indices;

    virtual void drawInspector() {

        ImGui::Text("Base Object");

        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
        ImGui::DragFloat3("Scale",     glm::value_ptr(scale),     0.1f);
    }

    virtual ~Object() = default;
    virtual void update(float dt) = 0;
    virtual void render(glm::mat4 view, glm::mat4 projection, Shader* defaultShader) = 0;
protected:
    std::vector<unsigned int*> VAO;
    std::vector<unsigned int*> VBO;
    std::vector<unsigned int*> EBO;
};

