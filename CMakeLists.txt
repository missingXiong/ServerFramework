cmake_minimum_required(VERSION 3.0)

project(WebServer CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURSE_DIR
    main.cpp
    ./src/Utility.cpp
    ./src/Channel.cpp
    ./src/Epoller.cpp
    ./src/EventLoop.cpp
    ./src/EventLoopThread.cpp
    ./src/EventLoopThreadPool.cpp
    ./src/ListenningSocket.cpp
    ./src/TcpConnection.cpp
    ./src/TcpServer.cpp
    ./src/ThreadSafeQueue.cpp
    ./src/ThreadPool.cpp)

set(CXX_FLAGS
    -g
    -Wall
    -std=c++11
    -pthread
    -Wno-unused-parameter)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(webserver ${SOURSE_DIR})
