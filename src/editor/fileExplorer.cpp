#include "fileExplorer.hpp"

#include "scriptEditor.hpp"
#include "projectLoader.hpp"

GLuint FileExplorer::LoadTextureFromFile(const char* filename, int* outWidth, int* outHeight)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) return 0;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    if (outWidth) *outWidth = width;
    if (outHeight) *outHeight = height;

    return textureID;
}

GLuint FileExplorer::GetPreviewTexture(const std::string& path) {
    if (imagePreviews.count(path)) return imagePreviews[path];

    GLuint tex = LoadTextureFromFile(path.c_str(), nullptr, nullptr);
    imagePreviews[path] = tex;
    return tex;
}

void FileExplorer::DrawFileExplorer(ProjectLoader& project) {
    ImGui::Begin("Project Explorer");

    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0)) {
        dragging = true;
        dragStart = ImGui::GetMousePos();
        dragEnd = dragStart;
        selectedFiles.clear();
    }

    if (dragging && ImGui::IsMouseDown(0)) {
        dragEnd = ImGui::GetMousePos();
    }

    if (dragging && ImGui::IsMouseReleased(0)) {
        dragging = false;
    }

    if (!initialized) {
        entries = project.LoadFolder(project.GetAbsoluteProjectPath());
        currentDir = project.GetRootFolder();
        initialized = true;
    }

    // Up button
    if (currentDir.has_parent_path()) {
        std::filesystem::path nodePath = currentDir;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2)); // tweak vertical padding
        if (currentDir != project.GetAbsoluteProjectPath()) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
            if (ImGui::Button((std::string(ICON_FA_CHEVRON_LEFT) + " Back").c_str())) {
                currentDir = currentDir.parent_path();
                entries = project.LoadFolder(currentDir);
            }
            ImGui::PopStyleVar();
        }
        if (ImGui::BeginDragDropTarget()) {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATHS")) {

                // Turn the payload into separate filenames
                const char* data = (const char*)payload->Data;
                std::stringstream ss(data);
                std::string item;

                while (std::getline(ss, item)) {
                    if (item.empty()) continue;

                    std::filesystem::path file = item;
                    std::filesystem::path dest = std::filesystem::absolute(nodePath.parent_path()) / file.filename();

                    std::error_code ec;
                    std::filesystem::rename(file, dest, ec);
                    ScriptEditors::OnFileMoved(file, dest);
                    if (ec) {
                        std::cout << "Move error: " << ec.message() << "\n";
                    }
                }

                refreshRequested = true;
            }

            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleVar();
    }


    ImGui::Spacing();

    float cellWidth = 100.0f; // width per icon cell, including spacing
    float availableWidth = ImGui::GetContentRegionAvail().x; // current window width

    int columns = (int)(availableWidth / cellWidth);
    if (columns < 1) columns = 1; // minimum 1 column
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 10));
    if (ImGui::BeginTable("filegrid", columns)) {
        for (size_t i = 0; i < entries.size(); i++) {
            auto& node = entries[i];

            ImGui::TableNextColumn();
            ImGui::PushID((int)i);

            // Simple icon
            const char* icon = node.isDirectory ? ICON_FA_FOLDER : ICON_FA_FILE;
            ImVec2 iconSize(60, 60);
            

            // Draw icon
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));           // transparent
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2,0.2,0.2,0.3));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2,0.2,0.2,0.5));

            // If it’s an image, show a preview
            std::string ext = node.name.substr(node.name.find_last_of(".") + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            GLuint texID = 0;
            if (!node.isDirectory && (ext == "png" || ext == "jpg" || ext == "jpeg")) {
                texID = GetPreviewTexture(node.absolutePath.c_str());
            }
            bool clicked = false;

            if (texID) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                clicked = ImGui::ImageButton(
                    std::to_string(texID).c_str(),
                    (void*)(intptr_t)texID,  // texture ID
                    iconSize,
                    ImVec2(0,1),              // UV0
                    ImVec2(1,0)              // UV1
                );
                ImGui::PopStyleVar();
            } else {
                // Fallback: normal button with icon
                clicked = ImGui::Button(icon, iconSize);
            }


            if (node.isDirectory) {
                if (ImGui::BeginDragDropTarget()) {

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATHS")) {

                        // Turn the payload into separate filenames
                        const char* data = (const char*)payload->Data;
                        std::stringstream ss(data);
                        std::string item;

                        while (std::getline(ss, item)) {
                            if (item.empty()) continue;

                            std::filesystem::path file = item;
                            std::filesystem::path dest = node.absolutePath / file.filename();

                            std::error_code ec;
                            std::filesystem::rename(file, dest, ec);
                            ScriptEditors::OnFileMoved(file, dest);
                            if (ec) {
                                std::cout << "Move error: " << ec.message() << "\n";
                            }
                        }

                        refreshRequested = true;
                    }

                    ImGui::EndDragDropTarget();
                }
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {

                // Build payload: multiple paths separated by '\n'
                std::string payloadStr;

                if (selectedFiles.count(node.relativePath)) {
                    // Dragging one of many selected → send all
                    for (auto& s : selectedFiles) {
                        payloadStr += s;
                        payloadStr += "\n";
                    }
                } else {
                    // Dragging single unselected file
                    payloadStr = node.relativePath + "\n";
                }

                ImGui::SetDragDropPayload(
                    "FILE_PATHS",
                    payloadStr.c_str(),
                    payloadStr.size() + 1
                );

                // Preview text
                if (selectedFiles.count(node.relativePath))
                    ImGui::Text("Moving %zu files", selectedFiles.size());
                else
                    ImGui::Text("Move %s", node.name.c_str());

                ImGui::EndDragDropSource();
            }


            // Marquee Selection Drag
            if (dragging) {
                ImRect box(
                    ImMin(dragStart, dragEnd),
                    ImMax(dragStart, dragEnd)
                );

                ImVec2 item_min = ImGui::GetItemRectMin();
                ImVec2 item_max = ImGui::GetItemRectMax();


                ImRect itemRect = ImRect(item_min, item_max);

                if (box.Overlaps(itemRect)) {
                    selectedFiles.insert(node.relativePath);
                }
            }

            // Click Selection
            if (clicked) {
                bool ctrl = ImGui::GetIO().KeyCtrl;
                bool shift = ImGui::GetIO().KeyShift;

                if (!ctrl && !shift)
                    selectedFiles.clear();

                selectedFiles.insert(node.relativePath);
            }

            // Highlight selected
            if (selectedFiles.count(node.relativePath)) {
                ImDrawList* dl = ImGui::GetWindowDrawList();

                ImVec2 min = ImGui::GetItemRectMin();
                ImVec2 max = ImGui::GetItemRectMax();

                // Light blue background
                dl->AddRectFilled(min, max, IM_COL32(100, 150, 255, 80), 4.0f);

                // Border
                dl->AddRect(min, max, IM_COL32(100, 150, 255, 200), 4.0f);
            }

            // File Context Window
            if (ImGui::BeginPopupContextItem("file_context")) {
                if (ImGui::MenuItem("Open")) {
                    std::cout << "Open " << node.relativePath << "\n";
                }
                if (ImGui::MenuItem("Rename")) {
                    std::cout << "Rename " << node.relativePath << "\n";
                }
                if (ImGui::MenuItem("Delete")) {
                    std::cout << "Delete " << node.relativePath << "\n";
                }
                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(3);

            // Handle double-click
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                if (node.isDirectory) {
                    currentDir = node.relativePath;
                    entries = project.LoadFolder(currentDir);
                    selectedFiles.clear();
                    ImGui::PopID();
                    ImGui::PopStyleVar();
                    ImGui::EndTable();
                    ImGui::End();
                    return;
                } else {
                    std::cout << "Open file: " << node.relativePath << "\n";
                    if (ext == "vs" || ext == "fs" || ext == "gs" || ext == "glsl") {
                        ScriptEditors::OpenScript(node.absolutePath, ScriptType::GLSL);
                    }
                }
            }

            // Name below icon
            ImGui::TextWrapped("%s", node.name.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    // Marquee Selection Drawing
    if (dragging) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImRect box(
            ImMin(dragStart, dragEnd),
            ImMax(dragStart, dragEnd)
        );

        dl->AddRectFilled(box.Min, box.Max, IM_COL32(100, 150, 255, 50));
        dl->AddRect(box.Min, box.Max, IM_COL32(100, 150, 255, 200));
    }

    ImGui::End();
    if (refreshRequested) {
        entries = project.LoadFolder(currentDir);
        selectedFiles.clear();
        refreshRequested = false;
    }
}