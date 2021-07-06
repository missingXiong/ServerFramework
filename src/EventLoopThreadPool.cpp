#include "../include/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* main, int threadnum)
    : threadNum_(threadnum),
      mainLoop_(main)
{
    for (int i = 0; i < threadNum_; i++)
    {
        EventLoopThread* loopThread = new EventLoopThread();
        threadList_.push_back(loopThread);
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    for (int i = 0; i < threadNum_; i++)
    {
        delete threadList_[i];
    }
    threadList_.clear();
}

void EventLoopThreadPool::start()
{
    if (threadNum_ <= 0) return;
    for (int i = 0; i < threadNum_; i++)
        threadList_[i]->start();
}

EventLoop* EventLoopThreadPool::getNextEventLoop()
{
    // if no IO thread, then using the main thread
    if (threadNum_ <= 0) return mainLoop_;

    // polling cycle strategy
    EventLoop* evLoop = threadList_[nextEventLoop_]->getEventLoop();
    nextEventLoop_ = (nextEventLoop_ + 1) % threadNum_;
    return evLoop;
}