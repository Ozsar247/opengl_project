#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <imgui/imgui.h>
#include <memory>

#include "../object.hpp"
#include "../texture.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

class MainEditorWindows {
public:
    inline static Object* selectedObject = nullptr;
    inline static ImVec2 lastMousePos = ImVec2(0, 0);

    static void RenderObjectExplorer(std::unordered_map<std::string, std::unique_ptr<Object>>& objects) {
        ImGui::Begin("Object Explorer");

        for (auto& [name, objPtr] : objects)
        {
            Object* obj = objPtr.get();

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_FramePadding |
                ImGuiTreeNodeFlags_Leaf;

            // Highlight if selected
            if (selectedObject == obj)
                flags |= ImGuiTreeNodeFlags_Selected;

            bool open = ImGui::TreeNodeEx(name.c_str(), flags);

            // Handle click → set selected
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                selectedObject = obj;
                std::cout << "Selected: " << name << "\n";
            }

            if (open)
                ImGui::TreePop();
        }

        ImGui::End();
    }
    
    static void TextureViewerRenderer() {
        ImGui::Begin("Textures");

        // --- Existing textures ---
        for (Texture* tex : Texture::getAllTextures()) {

            if (ImGui::ImageButton(
                    tex->path.c_str(),
                    (ImTextureID)(intptr_t)tex->texture,
                    ImVec2(200, 200),
                    ImVec2(0, 1),
                    ImVec2(1, 0))) 
            {
                const char* filterPatterns[] = { "*.png", "*.jpg", "*.jpeg", "*.bmp" };
                const char* file = tinyfd_openFileDialog(
                    "Select Texture",
                    "",
                    4,
                    filterPatterns,
                    "Image files",
                    0);

                if (file) {
                    tex->load(file);
                }
            }

            // Tooltip
            if (ImGui::IsItemHovered()) {
                std::string tooltip = tex->path + "\nTexture ID: " + std::to_string(tex->id);
                ImGui::SetTooltip("%s", tooltip.c_str());
            }
        }

        ImGui::Separator();

        // --- PLUS BUTTON FOR ADDING NEW TEXTURE ---
        if (ImGui::Button("+", ImVec2(200, 40))) {
            const char* filterPatterns[] = { "*.png", "*.jpg", "*.jpeg", "*.bmp" };
            const char* file = tinyfd_openFileDialog(
                "Add Texture",
                "",
                4,
                filterPatterns,
                "Image files",
                0);

            if (file) {
                // Allocate new texture
                Texture* newTex = new Texture();
                newTex->load(file);

                // Add to your global list (implement this)

            }
        }

        ImGui::End();
    }

    static void PropertiesRenderer() {
        ImGui::Begin("Properties");
        if (selectedObject) {
            selectedObject->drawInspector();
        } else {
            ImGui::Text("No Object Selected!");
        }  
        ImGui::End();
    }

    static void ViewportRenderer(int& s_x, int& s_y, unsigned int viewport, Camera& camera, GLFWwindow* window, Scene& scene) {
        const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        float currentFrame = glfwGetTime();
        static float lastFrame = currentFrame;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        windowSize.y -= titleBarHeight;
        s_x = windowSize.x;
        s_y = windowSize.y;
        ImVec2 mousePos = ImGui::GetIO().MousePos;


        ImVec2 centerPos = ImVec2(windowPos.x + windowSize.x / 2.0f, windowPos.y + windowSize.y / 2.0f);
        bool isHovered = ImGui::IsWindowHovered();


        // ---------------------------
        // Scroll / Zoom
        // ---------------------------
        if (isHovered) {
            ImGuiIO& io = ImGui::GetIO();
            if (io.MouseWheel != 0.0f) {
                camera.ProcessMouseScroll(io.MouseWheel); // positive = up, negative = down
            }
        }

        static bool firstFrame = true; // track first frame after cursor lock

        glm::mat4 view = camera.view;     // not strictly needed here
        glm::mat4 proj = camera.projection;

        if (isHovered && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
            // delta = mouse movement relative to center
            float xoffset = mousePos.x - centerPos.x;
            float yoffset = centerPos.y - mousePos.y; // invert Y-axis

            if (!firstFrame) { // ignore first frame delta (avoids jump after resetting)
                // apply to camera
                if (lastMousePos.x != mousePos.x || lastMousePos.y != mousePos.y) {
                    camera.ProcessMouseMovement(xoffset, yoffset);
                }

            }
            lastMousePos = mousePos;

            // reset cursor to center
            glfwSetCursorPos(window, centerPos.x, centerPos.y);

            firstFrame = false;
        } else {
            firstFrame = true; // reset when right button released
        }

        ImGui::Image((ImTextureID)(intptr_t)viewport, windowSize, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }


};