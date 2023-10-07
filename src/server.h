#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unordered_set>
#include "file_manager.h"
#include "thread_pool.h"

class Server {
public:
    Server(const char* ip, int port);
    void run();
    void handleClient(int client_socket);
    int epoll_fd;
    std::unordered_set<int> authenticated_clients;



private:
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    FileManager fileManager;
    ThreadPool pool;

    void log(const std::string &message);

    void sendFileResponse(int client_socket, const std::string &mimeType, const std::string &content);

    void sendErrorResponse(int client_socket, int statusCode, const std::string &message);

    void handleGET(int client_socket, const std::string &path);

    void handlePOST(int client_socket, const std::string &path, const std::string &fullRequest);
};

#endif // SERVER_H
