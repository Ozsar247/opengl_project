#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <filesystem>
#include <unordered_set>
#include <map>
#include <vector>
#include <string>

#include "fileNode.hpp"

// forward declaration
class ProjectLoader;

// fileExplorer.hpp
class FileExplorer {
public:
    void DrawFileExplorer(ProjectLoader& project);

private:
    GLuint LoadTextureFromFile(const char* filename, int* outWidth, int* outHeight);
    GLuint GetPreviewTexture(const std::string& path);

    inline static std::map<std::string, GLuint> imagePreviews;
    inline static std::unordered_set<std::string> selectedFiles;
    inline static bool dragging = false;
    inline static ImVec2 dragStart, dragEnd;
    inline static bool refreshRequested = false;

    inline static std::filesystem::path currentDir;
    inline static std::vector<FileNode> entries;
    inline static bool initialized = false;
};

