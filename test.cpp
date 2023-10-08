#include <iostream>
#include <filesystem> // 用于操作文件和目录
#include "cpp-httplib/httplib.h"
#include "src1/pool.h"
#include "src1/file_handler.h"
#include "src1/folder_handler.h"

//内联的一个常量，表示服务所在的工作目录根
inline const std::string SERVER_ROOT = "./uploads";


int main() {
    // 创建uploads目录
    std::filesystem::create_directories(SERVER_ROOT);

    //创建一个http服务实例
    httplib::Server svr;

    //一个无符号整数类型常量，用于计数或索引，非负，这里指线程数为5
    const size_t thread_count = 5;

    //创建一个容器，类型为线程，命名为工作器组，装这五个工作器用
    std::vector<std::thread> workers;

    //创建五个worker元素
    for (size_t i = 0; i < thread_count; ++i) {
        workers.emplace_back(worker);
    }

    //功能实现路由，服务的方法，格式以及执行程序。
    svr.Get("/files/(.*)", handle_file_download);
    svr.Post("/upload", handle_file_upload);
    svr.Post("/upload/(.*)", handle_file_upload);
    svr.Delete("/files/(.*)", handle_delete_file);
    svr.Put("/folders/(.*)", handle_create_folder);
    svr.Delete("/folders/(.*)", handle_delete_folder);

    //开始监听
    std::cout << "Server started at http://localhost:8090" << std::endl;
    svr.listen("0.0.0.0", 8090);

    //服务器停止后，等待所有线程join。
    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}

