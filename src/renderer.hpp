#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./IO/input.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

#include <string>
#include <functional>
#include <iostream>

class Renderer {
public:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        // Get your Renderer instance via glfwGetWindowUserPointer
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->SCR_W = width;
        renderer->SCR_H = height;
        for (auto* buf : framebuffers) {
            buf->ResizeFrameBuffer(width, height);
        }
        glViewport(0, 0, width, height);
    }
    inline static unsigned int SCR_W = 800;
    inline static unsigned int SCR_H = 600;

    Renderer(unsigned int width, unsigned int height, std::string name)
        : window(nullptr)
    {
        SCR_W = width;
        SCR_H = height;

        // GLFW init
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        if (!window) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        input = new Input(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        glEnable(GL_DEPTH_TEST);
        

        // Framebuffer resize callback
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        // ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        // Default clear color
        clearColor = glm::vec3(0.2f, 0.3f, 0.3f);
        clearBuffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    }

    void InsertFrameBuffer(Framebuffer* fb) {
        framebuffers.insert(fb);
    }

    // -------------------
    // MAIN RENDER LOOP
    // -------------------
    bool RenderLoop() {
        if (glfwWindowShouldClose(window)) return false;

        deltaTime = CalculateDeltaTime();

        input->update();
        if (inputCallback) inputCallback(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(clearBuffers);

        return true;
    }

    void RenderLast() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void SetClearColor(const glm::vec3& color) { clearColor = color; }
    void SetClearColor(float r, float g, float b) { clearColor = glm::vec3(r, g, b); }
    void SetClearBuffers(unsigned int buf) { clearBuffers = buf; }

    void SetInputProcessor(std::function<void(GLFWwindow*)> func) { inputCallback = func; }
    void SetMouseInputProcessor(GLFWcursorposfun func) { glfwSetCursorPosCallback(window, func); }
    void SetScrollInputProcessor(GLFWscrollfun func) { glfwSetScrollCallback(window, func); }

    ImGuiIO& GetImGuiIO() { return ImGui::GetIO(); }
    Input* GetInput() { return input; }
    GLFWwindow* GetWindow() { return window; }
    float GetDeltaTime() { return deltaTime; }

    bool getImGuiWantCaptureMouse() { return ImGui::GetIO().WantCaptureMouse; }
    bool getImGuiWantCaptureKeyboard() { return ImGui::GetIO().WantCaptureKeyboard; }

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
    float lastFrame = 0.0f;

    glm::vec3 clearColor;
    unsigned int clearBuffers;

    std::function<void(GLFWwindow*)> inputCallback;
    inline static std::unordered_set<Framebuffer*> framebuffers;

    float CalculateDeltaTime() {
        float currentFrame = glfwGetTime();
        float dt = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return dt;
    }
};
