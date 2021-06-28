#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "EventLoopThread.h"

class EventLoopThreadPool // IO thread pool
{
public:
    EventLoopThreadPool(EventLoop* main, 
        int threadnum = std::thread::hardware_concurrency());
    ~EventLoopThreadPool();

    void start();

private:
    EventLoop* getNextEventLoop();

private:
    int threadNum_;
    int nextEventLoop_;
    EventLoop* mainLoop_;
    std::vector<EventLoopThread*> threadList_;
};