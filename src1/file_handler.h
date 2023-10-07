#pragma once
#include <filesystem>
#include "../cpp-httplib/httplib.h"

void handle_file_upload(const httplib::Request& req, httplib::Response& res);
void handle_file_download(const httplib::Request& req, httplib::Response& res);
void handle_delete_file(const httplib::Request& req, httplib::Response& res);
