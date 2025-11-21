#pragma once

#include <glm/glm.hpp>
#include <shader.hpp>

struct DirLight {
    glm::vec3 direction;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;       
};

class Lights {
public:
    static void pointLight(int id, Shader& shader,glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) {
        shader.setVec3( "pointLights[" + std::to_string(id) + "].position", position);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].ambient", ambient);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].diffuse", diffuse);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].specular", specular);
        shader.setFloat("pointLights[" + std::to_string(id) + "].constant", constant);
        shader.setFloat("pointLights[" + std::to_string(id) + "].linear", linear);
        shader.setFloat("pointLights[" + std::to_string(id) + "].quadratic", quadratic);
    }

    static void pointLight(int id, Shader& shader, glm::vec3 position, glm::vec3 color, float intensity, float radius) {
        shader.setVec3( "pointLights[" + std::to_string(id) + "].position", position);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].ambient", color * intensity);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].diffuse", (color * intensity) * 0.1f);
        shader.setVec3( "pointLights[" + std::to_string(id) + "].specular", glm::vec3(1.0f) * intensity);
        shader.setFloat("pointLights[" + std::to_string(id) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(id) + "].linear", 4.5f / radius);
        shader.setFloat("pointLights[" + std::to_string(id) + "].quadratic", 75.0f / (radius * radius));
    }

    static void dirLight(Shader& shader, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
        shader.setVec3("dirLight.direction", direction);
        shader.setVec3("dirLight.ambient", ambient);
        shader.setVec3("dirLight.diffuse", diffuse);
        shader.setVec3("dirLight.specular", specular);
    }

    static void dirLight(Shader& shader, glm::vec3 direction, glm::vec3 color, float intensity) {
        shader.setVec3("dirLight.direction", direction);
        shader.setVec3("dirLight.ambient", (color * intensity) * 0.1f);
        shader.setVec3("dirLight.diffuse", color * intensity);
        shader.setVec3("dirLight.specular", glm::vec3(1.0f) * intensity);
    }

    static void spotLight(Shader& shader, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff) {
        shader.setVec3("spotLight.position", position);
        shader.setVec3("spotLight.direction", direction);
        shader.setVec3("spotLight.ambient", ambient);
        shader.setVec3("spotLight.diffuse", diffuse);
        shader.setVec3("spotLight.specular", specular);
        shader.setFloat("spotLight.constant", constant);
        shader.setFloat("spotLight.linear", linear);
        shader.setFloat("spotLight.quadratic", quadratic);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));     
    }

    static void spotLight(Shader& shader, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, float radius, float cutOffDegrees, float outerCutOffDegrees) {
        shader.setVec3("spotLight.position", position);
        shader.setVec3("spotLight.direction", direction);
        shader.setVec3("spotLight.ambient", (color * intensity) * 1.0f);
        shader.setVec3("spotLight.diffuse", color * intensity);
        shader.setVec3("spotLight.specular", glm::vec3(1.0f) * intensity);
        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 4.5f / radius);
        shader.setFloat("spotLight.quadratic", 75.0f / (radius * radius));
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(cutOffDegrees)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOffDegrees)));     
    }
};