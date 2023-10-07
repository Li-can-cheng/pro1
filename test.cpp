#include <iostream>
#include <filesystem> // 用于操作文件和目录
#include "cpp-httplib/httplib.h"
#include "src1/pool.h"
#include "src1/file_handler.h"
#include "src1/folder_handler.h"


inline const std::string SERVER_ROOT = "./uploads";


int main() {
    // 创建uploads目录
    std::filesystem::create_directories(SERVER_ROOT);

    httplib::Server svr;

    const size_t thread_count = 5;
    std::vector<std::thread> workers;
    for (size_t i = 0; i < thread_count; ++i) {
        workers.emplace_back(worker);
    }

    svr.Get("/files/(.*)", handle_file_download);
    svr.Post("/upload", handle_file_upload);
    svr.Post("/upload/(.*)", handle_file_upload);
    svr.Delete("/files/(.*)", handle_delete_file);
    svr.Put("/folders/(.*)", handle_create_folder);
    svr.Delete("/folders/(.*)", handle_delete_folder);




    std::cout << "Server started at http://localhost:8090" << std::endl;
    svr.listen("0.0.0.0", 8090);

    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}

