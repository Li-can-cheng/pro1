cmake_minimum_required(VERSION 3.22.1)
project(pro1)

set(CMAKE_CXX_STANDARD 17)

#add_executable(pro1 src(invalid)/main.cpp
#        src(invalid)/server.cpp
#        src(invalid)/server.h
#        src(invalid)/main.cpp
#        src(invalid)/file_manager.cpp
#        src(invalid)/file_manager.h
#        src(invalid)/thread_pool.h
#        src(invalid)/thread_pool.cpp)


add_executable(pro1 test.cpp
        src1/pool.cpp
        src1/pool.h
        src1/folder_handler.h
        src1/folder_handler.cpp
        src1/file_handler.h
        src1/file_handler.cpp
        src1/file_handler.h)
