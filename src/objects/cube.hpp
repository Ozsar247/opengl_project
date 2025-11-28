#pragma once

#include "../object.hpp"

#include "../shader.hpp"
#include "../bufferRenderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../editor/inspector.hpp"

#include "IconsFontAwesome6.h"


class Cube : public Object {
public:
    Texture* diffuse = nullptr;
    int diffuseID = 0;
    Texture* specular = nullptr;
    int specularID = 0;


    Cube() {
        cube.setVertices(cubeVertices);
        cube.setStride(sizeof(Vertex));

        cube.addAttribOffset(0, 3, GL_FLOAT, offsetof(Vertex, Position));
        cube.addAttribOffset(1, 3, GL_FLOAT, offsetof(Vertex, Normal));
        cube.addAttribOffset(2, 2, GL_FLOAT, offsetof(Vertex, TexCoords));
        // cube.addAttribOffset(3, 3, GL_FLOAT, offsetof(Vertex, Tangent));
        // cube.addAttribOffset(4, 3, GL_FLOAT, offsetof(Vertex, Bitangent));
        // cube.addAttribOffset(5, 4, GL_INT, offsetof(Vertex, m_BoneIDs));
        // cube.addAttribOffset(6, 4, GL_FLOAT, offsetof(Vertex, m_Weights));

        cube.link();

        VAO.push_back(cube.GetVAO());
        VBO.push_back(cube.GetVBO());

        obj_vertices.push_back(cubeVertices);

        explorer_icon = ICON_FA_CUBE;
    };
    
    void drawInspector() override {
        Object::drawInspector(); // draw base properties
        ImGui::Separator();
        ImGui::Text("Cube");
        Inspector::drawTextureField("Diffuse", diffuse, diffuseID);
        Inspector::drawTextureField("Specular", specular, specularID);
    }

    std::vector<Vertex> cubeVertices = {
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 1.f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 1.f}},
        {{-0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 0.f}},

        // Front face
        {{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
        {{ 0.5f, -0.5f, 0.5f}, {0.f, 0.f, 1.f}, {1.f, 0.f}},
        {{ 0.5f,  0.5f, 0.5f}, {0.f, 0.f, 1.f}, {1.f, 1.f}},
        {{ 0.5f,  0.5f, 0.5f}, {0.f, 0.f, 1.f}, {1.f, 1.f}},
        {{-0.5f,  0.5f, 0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
        {{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},

        // Left face
        {{-0.5f,  0.5f,  0.5f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},

        // Right face
        {{0.5f,  0.5f,  0.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{0.5f,  0.5f, -0.5f}, {1.f, 0.f, 0.f}, {1.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{0.5f, -0.5f,  0.5f}, {1.f, 0.f, 0.f}, {0.f, 0.f}},
        {{0.5f,  0.5f,  0.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.f, -1.f, 0.f}, {0.f, 1.f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.f, -1.f, 0.f}, {1.f, 1.f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
        {{-0.5f, -0.5f,  0.5f}, {0.f, -1.f, 0.f}, {0.f, 0.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.f, -1.f, 0.f}, {0.f, 1.f}},

        // Top face
        {{-0.5f,  0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 1.f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.f, 1.f, 0.f}, {1.f, 1.f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.f, 1.f, 0.f}, {1.f, 0.f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.f, 1.f, 0.f}, {1.f, 0.f}},
        {{-0.5f,  0.5f,  0.5f}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
        {{-0.5f,  0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 1.f}}
    };


    void update(float dt) override {}
    void render(glm::mat4 view, glm::mat4 projection, Shader* defaultShader) override {
        Shader* useShader = shader ? shader : defaultShader;
        if (!useShader) return;
        
        useShader->use();
        if (diffuse) {
            diffuse->bind(0, "material.diffuse");
        }
        if (specular) {
            specular->bind(1, "material.specular");
        }
        useShader->setFloat("material.shininess", 32.0f);
        useShader->setMat4("projection", projection);
        useShader->setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model *= glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );
        model = glm::scale(model, scale); 
        useShader->setMat4("model", model);
        cube.draw();
    }
    void SetTexture(const std::string& type, const std::string& path) override {
        if (type == "diffuse") {
            if (!diffuse) diffuse = new Texture();
            diffuse->load(path.c_str());
        } else if (type == "specular") {
            if (!specular) specular = new Texture();
            specular->load(path.c_str());
        }
        // etc.
    }
private:
    BufferRenderer cube;
};