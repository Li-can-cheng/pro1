#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
class FileManager {
public:
    FileManager(const std::string& root_directory);
    bool writeFile(const std::string& path, const std::string& content);
    std::string readFile(const std::string& path);
    bool deleteFileOrDirectory(const std::string& path);
    bool createDirectory(const std::string& path);

    std::vector<std::string> listDirectory(const std::string &path);

    std::string getMimeType(const std::string &filePath);

private:
    std::string rootDirectory;
    std::unordered_map<std::string, std::vector<std::string>> file_versions;

};

#endif
