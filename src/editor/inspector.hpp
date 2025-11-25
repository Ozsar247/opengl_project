#pragma once

#include "../texture.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include "../modelLoader.hpp"

class Inspector {
public:
    static void drawTextureField(const char* label, Texture*& tex, int& idBuffer) {
        ImGui::PushID(label); // <<< make all IDs inside unique

        ImGui::Text("%s", label);

        if (tex)
            ImGui::Text("Current ID: %d", tex->id);
        else
            ImGui::Text("Current ID: <none>");

        ImGui::InputInt("New Texture ID", &idBuffer);

        if (ImGui::Button("Load")) {   // ID = "label/Load"
            Texture* newTex = Texture::getById(idBuffer);
            
            if (newTex) tex = newTex;
        }

        if (tex && tex->texture)
            ImGui::Image((ImTextureID)(intptr_t)tex->texture, ImVec2(64, 64));

        ImGui::PopID();
    }

    static void drawModelField(const char* label, std::string& path, ModelLoader*& model) {
        ImGui::PushID(label); // <<< make all IDs inside unique

        ImGui::Text("%s", label);

        if (!path.empty())
            ImGui::Text("Current Path: %s", path.c_str());
        else
            ImGui::Text("Current Path: <none>");

        ImGui::InputText("New Model Path", &path);

        if (ImGui::Button("Load")) {   // ID = "label/Load"
            ModelLoader* newModel = new ModelLoader(path.c_str());
            
            if (newModel) model = newModel;
        }


        ImGui::PopID();
    }

};