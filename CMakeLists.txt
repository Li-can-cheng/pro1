cmake_minimum_required(VERSION 3.22.1)
project(pro1)

set(CMAKE_CXX_STANDARD 17)

#add_executable(pro1 src/main.cpp
#        src/server.cpp
#        src/server.h
#        src/main.cpp
#        src/file_manager.cpp
#        src/file_manager.h
#        src/thread_pool.h
#        src/thread_pool.cpp)


add_executable(pro1 test.cpp
        src1/pool.cpp
        src1/pool.h
        src1/folder_handler.h
        src1/folder_handler.cpp
        src1/file_handler.h
        src1/file_handler.cpp
        src1/file_handler.h)
