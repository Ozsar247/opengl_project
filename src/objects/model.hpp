#pragma once

#include "../object.hpp"
#include "../modelLoader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <string>

class Model : public Object {
public:
    Model(const std::string& modelPath) {
        path = modelPath;
        mod = new ModelLoader(path.c_str());
    }

    ~Model() {
        delete mod;
        mod = nullptr;
    }

    std::string path;

    void drawInspector() override {
        Object::drawInspector(); // draw base properties
        ImGui::Separator();
        ImGui::Text("Model Properties");
        
        Inspector::drawModelField("Model", path, mod);
    }

    void update(float dt) override {}

    void render(glm::mat4 view, glm::mat4 projection, Shader* defaultShader) override {
        Shader* useShader = shader ? shader : defaultShader;
        if (!useShader) return;

        useShader->use();
        useShader->setFloat("material.shininess", 32.0f);
        useShader->setMat4("projection", projection);
        useShader->setMat4("view", view);

        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, position);
        modelMat *= glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );
        modelMat = glm::scale(modelMat, scale); 
        useShader->setMat4("model", modelMat);

        if (mod)
            mod->Draw(*useShader);
    }

private:
    ModelLoader* mod = nullptr;
};
