#include "file_handler.h"
#include <iostream>
#include <fstream>

//从其他位置引入的常量字符串，代表服务器的根目录。
extern const std::string SERVER_ROOT;

//处理文件上传
void handle_file_upload(const httplib::Request& req, httplib::Response& res) {
    //获取名为file的文件
    auto file = req.get_file_value("file");

    // 从URL中获取目标文件夹路径
    std::string folder_path = req.matches[1];

//    为什么从req.matches[1]开始?
//
//    当你使用正则表达式进行匹配时，req.matches 通常会保存所有的匹配项。其中，req.matches[0] 通常保存整个匹配到的字符串，而 req.matches[1], req.matches[2], ... 则按顺序保存括号中的捕获组。
//    举例来说，如果你的路由模式是 "/upload/(.*)"（注意括号），那么对于URL "/upload/myfolder"，req.matches[0] 会是 "/upload/myfolder"，而 req.matches[1] 就是 "myfolder"。
//    所以，通常我们使用 req.matches[1] 来获取我们感兴趣的部分，即捕获组的内容。


//    另外，如果目标文件夹路径为 /upload（后面没有任何内容），所以 req.matches[1] 的值将是一个空字符串 ""。

    std::filesystem::create_directories(SERVER_ROOT + "/"+ folder_path); // 确保目标文件夹存在

    std::string save_path = SERVER_ROOT + "/" + folder_path + "/" + file.filename;
    std::ofstream ofs(save_path, std::ios::binary);
    ofs << file.content;

    std::cout << "已保存文件: " << save_path << " 大小: " << file.content.size() << " 字节" << std::endl;

    res.set_content("文件上传成功🎉", "text/plain");
}

void handle_file_download(const httplib::Request& req, httplib::Response& res) {
    std::string filename = req.matches[1];
    std::ifstream ifs(SERVER_ROOT + "/" + filename, std::ios::binary);
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
    if (std::filesystem::remove(SERVER_ROOT + "/" + filename)) {
        res.set_content("文件删除成功🎉", "text/plain");
    } else {
        res.status = 404;
        res.set_content("文件删除失败😞", "text/plain");
    }
}