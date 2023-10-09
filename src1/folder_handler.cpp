#include "folder_handler.h"
#include <filesystem>

extern const std::string SERVER_ROOT;


void handle_create_folder(const httplib::Request& req, httplib::Response& res) {
    std::string foldername = req.matches[1];
    if (std::filesystem::create_directory(SERVER_ROOT + "/" + foldername)) {
        res.set_content("文件夹创建成功🎉", "text/plain");
    } else {
        res.set_content("文件夹创建失败,可能已经有这个文件夹了😞", "text/plain");
    }
}


void handle_delete_folder(const httplib::Request& req, httplib::Response& res) {
    std::string foldername = req.matches[1];
    if (std::filesystem::remove_all(SERVER_ROOT + "/" + foldername)) {
        res.set_content("文件夹删除成功🎉", "text/plain");
    } else {
        res.set_content("文件夹删除失败😞", "text/plain");
    }
}