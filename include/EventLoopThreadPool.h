#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "EventLoopThread.h"

class EventLoopThreadPool // IO thread pool
{
public:
    EventLoopThreadPool(EventLoop* main, int threadnum);
    ~EventLoopThreadPool();

    void start();

    EventLoop* getNextEventLoop();

private:
    int threadNum_;
    int nextEventLoop_;
    EventLoop* mainLoop_;
    std::vector<EventLoopThread*> threadList_;
};