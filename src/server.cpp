#include <thread>
#include <sys/epoll.h>
#include "server.h"
#include "file_manager.h"
using namespace  std;

const ssize_t BUFFER_SIZE = 65536; // 64KB
const size_t MAX_UPLOAD_SIZE = 10 * 1024 * 1024; // 例如，设置为10MB

Server::Server(const char* ip, int port)
        : pool(4),  // 初始化线程池，启动4个线程
          fileManager("cloud_storage_files")  // 初始化 fileManager，设置其根目录为"cloud_storage_files"

{
    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "创建socket失败" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "创建socket成功😀" << std::endl;
    }

    // 设置 socket 选项，以便重新使用地址和端口
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt失败😱" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "setsockopt成功😀" << std::endl;
    }

    // 设置地址结构体
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);  // 使用提供的IP地址
    address.sin_port = htons(port);

    // 绑定 socket 到地址
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "绑定失败😓" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "绑定成功😀" << std::endl;
    }

}


void Server::run() {
    // 开始监听
    if (listen(server_fd, 5) < 0) {
        std::cerr << "监听失败😓" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "监听中...👂" << std::endl;
    }
// 初始化 epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "epoll创建失败😓" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        std::cerr << "epoll_ctl失败😓" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 替换原始的while循环
    struct epoll_event events[10];
    while (true) {
        int num_fds = epoll_wait(epoll_fd, events, 10, -1);
        for (int i = 0; i < num_fds; i++) {
            if (events[i].data.fd == server_fd) {
                // 新连接到达
                int client_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
                if (client_socket == -1) {
                    std::cerr << "接受连接失败😢" << std::endl;
                    continue;
                }
                std::cout << "新的连接建立🌟" << std::endl;

                // 将新的socket添加到epoll的监听事件中
                ev.events = EPOLLIN;
                ev.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1) {
                    std::cerr << "epoll_ctl添加客户端失败😓" << std::endl;
                    continue;
                }
            } else {
                // 从客户端读取数据
                int client_socket = events[i].data.fd;
                pool.enqueue([this, client_socket]() {
                    this->handleClient(client_socket);
                });
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, nullptr);


            }
        }
    }
}


void Server::handleClient(int client_socket) {
    struct timeval tv;
    tv.tv_sec = 5;  // 设置5秒的超时
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    std::string fullRequest;  // 用于组合完整的请求
    char buffer[BUFFER_SIZE];

    while (true) {
        ssize_t bytesRead = read(client_socket, buffer, sizeof(buffer) - 1);  // 为 '\0' 留下空间
        if (bytesRead <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "读取数据超时😢" << std::endl;
            } else {
                std::cerr << "从客户端读取数据时发生错误😓, 原因: " << strerror(errno) << std::endl;
            }
            close(client_socket);
            return;
        }
        buffer[bytesRead] = '\0';  // 确保字符串结尾
        fullRequest += buffer;
        if(fullRequest.find("\r\n\r\n") != std::string::npos) {
            break;  // HTTP请求头部和主体之间的分隔符，表示请求已完整
        }
    }

    // 提取HTTP方法和路径
    std::istringstream requestStream(fullRequest);
    std::string method, path;
    requestStream >> method >> path;

    // 处理请求
    if (method == "POST") {
        handlePOST(client_socket, path, fullRequest);
    } else if (method == "GET") {
        handleGET(client_socket, path);
    } else {
        sendErrorResponse(client_socket, 405, "不支持的HTTP方法😓");
    }

    close(client_socket);
}
void Server::handleGET(int client_socket, const std::string& path) {
    std::string fileContent = fileManager.readFile(path.substr(1));
    if (fileContent.empty()) {
        sendErrorResponse(client_socket, 404, "文件未找到😞");
    } else {
        std::string mimeType = fileManager.getMimeType(path.substr(1));
        sendFileResponse(client_socket, mimeType, fileContent);
    }
}


void Server::handlePOST(int client_socket, const std::string& path, const std::string& fullRequest) {
    // 假设边界的格式为: ------------------------xxxxx
    std::cout << "完整请求: \n" << fullRequest << std::endl;

    size_t boundaryStart = fullRequest.find("------------------------");
    if(boundaryStart == std::string::npos) {
        sendErrorResponse(client_socket, 400, "找不到multipart边界😓");
        return;
    }

    // 获取完整边界字符串
    size_t boundaryEnd = fullRequest.find("\r\n", boundaryStart);
    std::string boundary = fullRequest.substr(boundaryStart, boundaryEnd - boundaryStart);

    // 查找文件内容的开始位置
    size_t contentStart = fullRequest.find("\r\n\r\n", boundaryEnd) + 4;

    // 查找文件内容的结束位置 (即下一个边界的开始位置)
    size_t contentEnd = fullRequest.find(boundary, contentStart);

    // 提取文件内容
    std::string fileContent = fullRequest.substr(contentStart, contentEnd - contentStart - 2); // -2 to remove the \r\n before the boundary

    if (fileContent.size() > MAX_UPLOAD_SIZE) {
        sendErrorResponse(client_socket, 413, "上传的文件太大😓");
        return;
    }

    // 保存文件到服务器
    if(fileManager.writeFile(path.substr(1), fileContent)) {
        std::string message = "文件成功上传🎉";
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + std::to_string(message.size()) + "\r\n";
        response += "\r\n";
        response += message;
        write(client_socket, response.c_str(), response.size());
    } else {
        sendErrorResponse(client_socket, 500, "文件上传失败😢");
    }
}




void Server::sendErrorResponse(int client_socket, int statusCode, const std::string& message) {
    std::string statusMessage;
    switch(statusCode) {
        case 400: statusMessage = "Bad Request"; break;
        case 404: statusMessage = "Not Found"; break;
        case 405: statusMessage = "Method Not Allowed"; break;
        default: statusMessage = "Internal Server Error"; statusCode = 500;
    }

    std::string response = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + std::to_string(message.size()) + "\r\n";
    response += "\r\n";
    response += message;
    write(client_socket, response.c_str(), response.size());
}

void Server::sendFileResponse(int client_socket, const std::string& mimeType, const std::string& content) {
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + mimeType + "\r\n";
    response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    response += "\r\n";
    response += content;
    write(client_socket, response.c_str(), response.size());
}






