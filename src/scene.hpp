#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

#include "object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene {
public:
    glm::mat4 view;
    glm::mat4 projection;
    bool wireframe = false;

    Scene() {}

    void addObject(const std::string& name, std::unique_ptr<Object> obj) {
        objects[name] = std::move(obj);
    }

    void removeObject(const std::string& name) {
        objects.erase(name);
    }

    void update(float dt) {
        for (auto& [name, obj] : objects) {
            //obj->update(dt);
        }
    }

    

    void render() {
        Shader* defaultShader = Shader::getCurrentShader();

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {   
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        for (auto& [name, obj] : objects) {
            obj->render(view, projection, defaultShader);
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    template <typename T, typename... Args>
    static std::unique_ptr<T> NewInstance(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    std::unordered_map<std::string, std::unique_ptr<Object>>& Objects() {
        return objects;
    }

    Object* getObject(const std::string& name) {
        auto it = objects.find(name);
        return it != objects.end() ? it->second.get() : nullptr;
    }
private:
    std::unordered_map<std::string, std::unique_ptr<Object>> objects;
};