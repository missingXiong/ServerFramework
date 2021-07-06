#pragma once

#include <iostream>
#include <thread>

#include "EventLoop.h"

class EventLoopThread // EventLoop runs in EventLoopThread(IO thread)
{
public:
    EventLoopThread();
    ~EventLoopThread();

    void start();
    EventLoop* getEventLoop() const { return this->loop_; }

private:
    void run();

private:
    EventLoop* loop_;
    std::thread thread_;
    
};