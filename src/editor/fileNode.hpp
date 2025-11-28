#pragma once
#include <string>
#include <vector>

struct FileNode {
    std::string name;
    std::string relativePath;
    std::string absolutePath;
    bool isDirectory;
    std::vector<FileNode> children;
};