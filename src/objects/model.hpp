#pragma once

#include "../object.hpp"
#include "../modelLoader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>


class Model : public Object {
public:
    Model(std::string path) : mod((char*)path.c_str()) {};

    void update(float dt) override {}
    void render(glm::mat4 view, glm::mat4 projection, Shader* shader) override {
        shader->use();
        shader->setFloat("material.shininess", 32.0f);
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model *= glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );
        model = glm::scale(model, scale); 
        shader->setMat4("model", model);
        mod.Draw(*shader);
    }
private:
    ModelLoader mod;
};