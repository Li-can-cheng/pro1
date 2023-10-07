#include "file_handler.h"
#include <iostream>
#include <fstream>

extern const std::string SERVER_ROOT;

// ... (所有文件处理相关函数的实现) ...
void handle_file_upload(const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");

    // 从URL中获取目标文件夹路径
    std::string folder_path = req.matches[1];
    std::filesystem::create_directories(SERVER_ROOT + folder_path); // 确保目标文件夹存在

    std::string save_path = SERVER_ROOT + folder_path + "/" + file.filename;
    std::ofstream ofs(save_path, std::ios::binary);
    ofs << file.content;

    std::cout << "已保存文件: " << save_path << " 大小: " << file.content.size() << " 字节" << std::endl;

    res.set_content("文件上传成功🎉", "text/plain");
}

void handle_file_download(const httplib::Request& req, httplib::Response& res) {
    std::string filename = req.matches[1];
    std::ifstream ifs(SERVER_ROOT + filename, std::ios::binary);
    if (ifs) {
        std::string ext = std::filesystem::path(filename).extension();
        std::string mimeType = "application/octet-stream"; // 默认类型

        if (ext == ".jpg" || ext == ".jpeg") {
            mimeType = "image/jpeg";
        } else if (ext == ".png") {
            mimeType = "image/png";
        }

        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        res.set_content(content, mimeType);
    } else {
        res.status = 404;
        res.set_content("文件未找到😞", "text/plain");
    }
}

void handle_delete_file(const httplib::Request& req, httplib::Response& res) {
    std::string filename = req.matches[1];
    if (std::filesystem::remove(SERVER_ROOT + filename)) {
        res.set_content("文件删除成功🎉", "text/plain");
    } else {
        res.status = 404;
        res.set_content("文件删除失败😞", "text/plain");
    }
}