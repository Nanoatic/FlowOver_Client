cmake_minimum_required(VERSION 3.8)
project(FlowOver_Client)
find_package(Threads)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES sync_queue.h includes.h helper_class.h helper_class.cpp constants_and_definitions.h main.cpp)
add_executable(FlowOver_Client ${SOURCE_FILES})
target_link_libraries(FlowOver_Client ${CMAKE_THREAD_LIBS_INIT})