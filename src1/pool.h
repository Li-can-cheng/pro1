#pragma once
#include "../cpp-httplib/httplib.h"
#include <functional>
#include <queue>
#include <mutex>
#include <thread>

template <typename T>
class ThreadSafeQueue;

struct Task {
    httplib::Request req;
    httplib::Response res;
    std::function<void(const httplib::Request&, httplib::Response&)> handler;
};

void worker();

extern ThreadSafeQueue<Task> tasks;
