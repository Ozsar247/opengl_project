#pragma once

#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <variant>
#include <map>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

#include "IconsFontAwesome6.h"
#include "scene.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "object.hpp"  // defines Object, Texture
#include "texture.hpp"
#include "./objects/model.hpp"
#include "./objects/cube.hpp"

#include "fileNode.hpp"

#include "./fileExplorer.hpp"

#include <stb/stb_image.h>

using ArgType = std::variant<int, float, std::string>;

enum FileType {
    IMAGE,
    SHADER,
    CODE,
    FOLDER
};


class ProjectLoader {
public:
    ProjectLoader(const std::string& projectFilePath) : projectFilePath(projectFilePath) {
        std::string projectXml;
        std::ifstream xmlFile;

        xmlFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            xmlFile.open(projectFilePath);
            std::stringstream xmlStream;

            xmlStream << xmlFile.rdbuf();
            xmlFile.close();

            projectXml = xmlStream.str();
        } catch (std::ifstream::failure& err) {
            std::cout << "ERROR: Project File Read Error: " << projectFilePath << std::endl;
            return;
        }

        // RapidXML needs mutable char* buffer
        buffer.assign(projectXml.begin(), projectXml.end());
        buffer.push_back('\0'); // null-terminate

        doc.parse<0>(&buffer[0]);

        //xml_node<> *bin = doc.first_node()->first_node("project_info")->first_node("bin_folder");

        std::filesystem::path projectPath = projectFilePath;
        root_folder = projectPath.parent_path();
    }

    FileNode LoadFilesRecursive(const std::filesystem::path& path) {
        FileNode node;
        node.name = path.filename().string();
        node.relativePath = path.string();
        node.isDirectory = std::filesystem::is_directory(root_folder / path);
        node.absolutePath = (std::filesystem::absolute(root_folder / path)).string();

        if (node.isDirectory) {
            for (auto& entry : std::filesystem::directory_iterator(root_folder / path)) {
                node.children.push_back(LoadFilesRecursive(entry.path()));
            }
        }

        return node;
    }

    std::vector<FileNode> LoadFolder(const std::filesystem::path& path) {
        std::vector<FileNode> nodes;

        for (auto& entry : std::filesystem::directory_iterator(path)) {
            FileNode node;
            node.name = entry.path().filename().string();
            node.relativePath = entry.path().string();
            node.isDirectory = entry.is_directory();
            node.absolutePath = (std::filesystem::absolute(entry.path())).string();
            nodes.push_back(node);
        }

        // Optional: sort folders first, then files
        std::sort(nodes.begin(), nodes.end(),
                [](const FileNode& a, const FileNode& b) {
                    if (a.isDirectory != b.isDirectory)
                        return a.isDirectory; // directories first
                    return a.name < b.name;
                });

        return nodes;
    }

    Scene loadSceneFromXML(Renderer* render) {
        Scene newScene;
        LoadShaderPrograms(render);
        xml_node<> *node = doc.first_node()->first_node("world");
        for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
        {
            std::map<std::string, ArgType> args;
            xml_node<> *argNode = child->first_node("args");
            if (argNode) {
                for (xml_node<> *arg = argNode->first_node(); arg; arg = arg->next_sibling()) {
                    args[arg->name()] = std::string(arg->value());
                }
            }
            args["type"] =  child->first_attribute("type")->value();
            auto obj = typeFactory(newScene, args);


            std::vector<Texture> textures;
            std::vector<Shader> shaders;
            std::cout << child->name() << std::endl;
            for (xml_node<> *child2 = child->first_node(); child2; child2 = child2->next_sibling())
            {
                std::string propName = child2->name();
                if (propName == "transform") {
                    auto* posNode = child2->first_node("position");
                    auto* rotNode = child2->first_node("rotation");
                    auto* scaleNode = child2->first_node("scale");

                    obj->position = glm::vec3(
                        GetAttrF(posNode, "x"),
                        GetAttrF(posNode, "y"),
                        GetAttrF(posNode, "z")
                    );

                    obj->rotation = glm::vec3(
                        GetAttrF(rotNode, "x"),
                        GetAttrF(rotNode, "y"),
                        GetAttrF(rotNode, "z")
                    );

                    obj->scale = glm::vec3(
                        GetAttrF(scaleNode, "x", 1.0f),
                        GetAttrF(scaleNode, "y", 1.0f),
                        GetAttrF(scaleNode, "z", 1.0f)
                    );
                } else if (propName == "textures") {
                   for (xml_node<> *tex = child2->first_node(); tex; tex = tex->next_sibling()) {
                        std::string type = tex->first_attribute("type")->value();
                        std::string path = (root_folder / tex->first_attribute("path")->value()).string();;

                        obj->SetTexture(type, path);
                    }
                } else if (propName == "shader_program") {
                    int shaderId = std::stoi(child2->first_attribute("id")->value());
                    obj->shader = render->GetGlobalShader(shaderId); // works because both are Shader*
                } else if (propName == "scripts") {
                    // ill do this later TODO
                } else {
                    continue;
                }
            }

            newScene.addObject(child->first_attribute("name")->value(), std::move(obj));
        }
        return newScene;
    }

    void LoadShaderPrograms(Renderer* render) {
        xml_node<>* node = doc.first_node()->first_node("shader_programs");
        if (!node) return;

        for (xml_node<>* program = node->first_node(); program; program = program->next_sibling()) {
            int id = std::stoi(program->first_attribute("id")->value());
            std::filesystem::path vertexPath = root_folder / program->first_attribute("vertex")->value();
            std::filesystem::path fragmentPath = root_folder / program->first_attribute("fragment")->value();
            const char* geom = program->first_attribute("geometry") 
                ? (root_folder / program->first_attribute("geometry")->value()).c_str() 
                : nullptr;

            Shader* s = new Shader(vertexPath.c_str(), fragmentPath.c_str(), geom);
            
            render->AddGlobalShader(id, s);
            if (program->first_attribute("vertex")->value()) {
                render->setDefaultShader(s);
            }
        }
    }

    void RenderFileExplorer() {
        FileExplorer explorer;
        explorer.DrawFileExplorer(*this);
    }
    

    std::filesystem::path GetAbsoluteProjectPath() {
        return std::filesystem::absolute(root_folder);
    }

    std::filesystem::path GetRootFolder() {
        return root_folder;
    }



private:
    xml_document<> doc;
    std::vector<char> buffer;
    std::filesystem::path projectFilePath;
    std::filesystem::path root_folder;

    

    std::unique_ptr<Object> typeFactory(Scene& scene, const std::map<std::string, ArgType>& args) {
        static std::unordered_map<std::string, std::function<std::unique_ptr<Object>()>> factory;

        if (factory.empty()) {
            factory["Model"] = [this, &scene, &args]() -> std::unique_ptr<Object> {
                auto it = args.find("path");
                if (it == args.end()) {
                    std::cerr << "Error: Model missing 'path' argument!\n";
                    return nullptr;
                }

                const std::string& pathStr = std::get<std::string>(it->second);
                std::filesystem::path fullPath = root_folder / pathStr;

                return scene.NewInstance<Model>(fullPath);
            };

            factory["Cube"] = [&scene]() -> std::unique_ptr<Object> {
                return scene.NewInstance<Cube>();
            };
        }

        auto it = args.find("type");
        if (it == args.end()) {
            std::cerr << "Error: Missing 'type' in object args\n";
            return nullptr;
        }

        const std::string& type = std::get<std::string>(it->second);

        auto fIt = factory.find(type);
        if (fIt != factory.end())
            return fIt->second();

        std::cerr << "Error: Unknown object type '" << type << "'\n";
        return nullptr;
    }

    inline float GetAttrF(xml_node<>* node, const char* name, float def = 0.0f) {
        auto* attr = node->first_attribute(name);
        return attr ? std::stof(attr->value()) : def;
    }

};
