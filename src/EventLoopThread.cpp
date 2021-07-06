#include "../include/EventLoopThread.h"

EventLoopThread::EventLoopThread()
    : loop_(nullptr)
{

}

EventLoopThread::~EventLoopThread()
{
    if (loop_ == nullptr) return;

    loop_->quit();
    if (thread_.joinable())
        thread_.join();
}

void EventLoopThread::start()
{
    thread_ = std::thread(&EventLoopThread::run, this);
}

void EventLoopThread::run()
{
    EventLoop evLoop;
    loop_ = &evLoop;
    loop_->loop();
}