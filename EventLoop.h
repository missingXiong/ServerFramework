#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <mutex>

#include "noncopyable.h"
#include "Channel.h"
#include "Epoller.h"

// a class for IO multiplexing
// the main purpose of this class is allow main thread adds tasks into this eventloop
class EventLoop : public noncopyable 
{
public:
    typedef std::function<void()> Task;
public:
    EventLoop();
    ~EventLoop();

    void quit();
    void loop(); 
    
    void addChannelToEpoller(Channel* pChannel); // don't call from other thread
    void removeChannelFromEpoller(Channel* pChannel); // don't call from other thread
    void updateChannelToEpoller(Channel* pChannel); // don't call from other thread

    std::thread::id getThreadId() const { return threadId_;}

    // callback for weakupChannel
    void handleRead();
    void handleError();

    void runInLoop(Task&& task); // safe to call from other thread, prefer to using this one
    
private:
    bool isInLoopThread();
    void wakeup();
    void executePendingTasks();
    void addPendingTask(Task&& task); // safe to call from other thread

private:
    bool quit_; // stop/start the eventloop
    int wakeupfd_;
    Channel weakupChannel_; // weakup eventloop avoiding blocking on epoll_wait
    Epoller epoller_; 
    std::thread::id threadId_; // the thread id where eventloop belong to
    std::mutex mutex_;
    bool callingPendingTasks_;
    std::vector<Task> pendingTasks; // request add/update/remove channel through task
    std::vector<Channel*> activeChannelList_; // 
};