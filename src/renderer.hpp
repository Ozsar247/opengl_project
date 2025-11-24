#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./IO/input.hpp"

#include <string>
#include <iostream>


class Renderer {
public:
    Renderer(unsigned int width, unsigned int height, std::string name)
        : window(nullptr)   // <-- initialize reference here
    {
        // GLFW Loader
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        if (!window) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        input = new Input(window);

        // GLAD loader
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        glEnable(GL_DEPTH_TEST);

        // ImGUI init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // ImGuiIO is already initialized above
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    GLFWwindow* GetWindow() {
        return window;
    }

    ImGuiIO& GetImGuiIO() {
        return ImGui::GetIO();
    }

    Input* GetInput() {
        return input;
    }

    float GetDeltaTime() {
        return deltaTime;
    }
    
    void SetClearColor(glm::vec3 color) {
        clearColor = color;
    } 

    void SetClearColor(float r, float g, float b) {
        clearColor = glm::vec3(r,g,b);
    }

    void SetInputProcessor(std::function<void(GLFWwindow*)> func) {
        inputCallback = func;
    }

    void SetMouseInputProcessor(GLFWcursorposfun func) {
        glfwSetCursorPosCallback(window, func);
        
    }

    void SetScrollInputProcessor(GLFWscrollfun func) {
        glfwSetScrollCallback(window, func);
    }

    bool RenderLoop() {
        if (glfwWindowShouldClose(window)) {
            return false;
        }

        deltaTime = CalculateDeltaTime();

        input->update();

        if (inputCallback) {
            inputCallback(window);
        }

        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(clearBuffers); // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
        return true;
    }

    void RenderLast() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void SetClearBuffers(unsigned int buf) {
        clearBuffers = buf;
    }

    bool getImGuiWantCaptureMouse() {
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool getImGuiWantCaptureKeyboard() {
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    void Cleanup() {
        delete input;
        input = nullptr;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }

private:
    GLFWwindow* window;
    Input* input;

    float deltaTime = 0.0f;
    float CalculateDeltaTime() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }
    float lastFrame = 0.0f;

    glm::vec3 clearColor = {0.2f, 0.3f, 0.3f};

    std::function<void(GLFWwindow*)> inputCallback;

    unsigned int clearBuffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
};
