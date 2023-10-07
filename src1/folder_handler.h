#pragma once
#include "../cpp-httplib/httplib.h"

void handle_create_folder(const httplib::Request& req, httplib::Response& res);
void handle_delete_folder(const httplib::Request& req, httplib::Response& res);
