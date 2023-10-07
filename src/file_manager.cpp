#include <vector>
#include <iostream>
#include "file_manager.h"


FileManager::FileManager(const std::string& root_directory) : rootDirectory(root_directory) {
    std::filesystem::create_directories(rootDirectory);  // Ensure root directory exists
}


bool FileManager::writeFile(const std::string& path, const std::string& content) {
    if (path.find("..") != std::string::npos) {
        std::cerr << "错误: 路径包含非法字符！" << std::endl;
        return false;
    }

    std::ofstream file(rootDirectory + "/" + path);
    if (file) {
        file << content;
        file.close();  // 👈 关闭文件
        return true;
    }
    return false;
}

std::string FileManager::readFile(const std::string& path) {
    if (path.find("..") != std::string::npos) {
        std::cerr << "错误: 路径包含非法字符！" << std::endl;
        return "";  // 或返回空字符串
    }
    std::filesystem::path fullPath = std::filesystem::path(rootDirectory) / path;

    // 检查是否是目录
    if (std::filesystem::is_directory(fullPath)) {
        std::cerr << "错误: '" << path << "' 是一个目录，不能读取！" << std::endl;
        return "";
    }

    std::ifstream file(fullPath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << path << std::endl;
        return "";
    }

    std::string content;
    try {
        content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    } catch (const std::exception& e) {
        std::cerr << "读取文件'" << path << "'时出现异常: " << e.what() << std::endl;
        file.close();
        return "";
    }
    file.close();
    return content;
}


std::string FileManager::getMimeType(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::string extension = path.extension().string();

    static std::unordered_map<std::string, std::string> mimeMap = {
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".png", "image/png"},
            // 添加其他文件类型和MIME类型映射...
    };

    auto it = mimeMap.find(extension);
    if (it != mimeMap.end()) {
        return it->second;
    } else {
        return "application/octet-stream";  // 通用二进制数据类型
    }
}


