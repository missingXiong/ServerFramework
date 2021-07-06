#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "../include/EventLoop.h"

EventLoop::EventLoop()
    : quit_(false),
      callingPendingTasks_(false),
      threadId_(std::this_thread::get_id())
{
    auto createWeakupfd = [this](){
        this->wakeupfd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (this->wakeupfd_ < 0)
        {
          perror("create weakup fd error");
          exit(1);
        }
    };

    createWeakupfd();
    weakupChannel_.setfd(wakeupfd_);
    weakupChannel_.setEvents(EPOLLIN | EPOLLET);
    weakupChannel_.setReadHandler(std::bind(&EventLoop::handleRead, this));
    weakupChannel_.setErrorhandler(std::bind(&EventLoop::handleError, this));
    addChannelToEpoller(&weakupChannel_);
}

EventLoop::~EventLoop()
{
    close(wakeupfd_);
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
        wakeup();
}

void EventLoop::loop()
{
    while (!quit_)
    {
        std::vector<Channel*> activeChannels;
        epoller_.poll(activeChannels);
        for (auto channel : activeChannels)
            channel->handleEvents();
        
        activeChannels.clear();
        executePendingTasks();
    }
}

void EventLoop::addChannelToEpoller(Channel *pChannel)
{
    epoller_.addChannel(pChannel);
}

void EventLoop::removeChannelFromEpoller(Channel *pChannel)
{
    epoller_.removeChannel(pChannel);
}

void EventLoop::updateChannelToEpoller(Channel *pChannel)
{
    epoller_.updateChannel(pChannel);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    write(wakeupfd_, (char*)(&one), sizeof one);
}

// callback for weakupChannel
void EventLoop::handleRead()
{
    uint64_t one = 1;
    read(wakeupfd_, &one, sizeof one);
}

void EventLoop::handleError()
{
}

void EventLoop::runInLoop(Task&& task)
{
    if (isInLoopThread())
        task();
    else
        addPendingTask(std::move(task));
}

void EventLoop::addPendingTask(Task&& task)
{
    {
        std::lock_guard<std::mutex> mut(mutex_);
        pendingTasks.emplace_back(std::move(task));
    }

    // if the function is invoked by other threads or execute pending tasks
    if (!isInLoopThread() || callingPendingTasks_) 
        wakeup();
}

void EventLoop::executePendingTasks()
{
    std::vector<Task> tasks;
    callingPendingTasks_ = true;

/* don't use in this way, because new task may be added into pendingTasks by other threads
    {
        std::lock_guard<std::mutex> mut(mutex_);
        for (auto& task : pendingTasks)
            task();
    }   */

    {
        std::lock_guard<std::mutex> mut(mutex_);
        tasks.swap(pendingTasks);
    }

    for (auto& task : tasks)
        task();

    callingPendingTasks_ = false;
}

bool EventLoop::isInLoopThread()
{
    return std::this_thread::get_id() == threadId_;
}