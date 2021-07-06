#include <unistd.h>
#include <sys/epoll.h>
#include <memory>
#include "Epoller.h"

const int EVENTNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoller::Epoller()
    : eventList_(EVENTNUM)
{
    epollfd_ = epoll_create(5);
    if (epollfd_ == -1)
    {
        perror("epoll create error");
        exit(1);
    }
}

Epoller::~Epoller()
{
    close(epollfd_);
}

void Epoller::poll(ChannelList &activeChannelList)
{
    // remember to setup timeout
    int nfds = epoll_wait(epollfd_, &*eventList_.begin(), eventList_.size(), EPOLLWAIT_TIME);
    if (nfds < 0)
        perror("epoll wait error");
    
    for (int i = 0; i < nfds; i++)
    {
        int events = eventList_[i].events;
        Channel* pChannel = static_cast<Channel*>(eventList_[i].data.ptr);
        int fd = pChannel->getfd();

        // std::unordered_map<int, Channel*>::const_iterator iter;
        // {
        //     std::lock_guard<std::mutex> mut(mutex_);
        //     iter = channelMap_.find(fd);
        // }

        if (channelMap_.find(fd) != channelMap_.end())
        {
            pChannel->setEvents(events);
            activeChannelList.push_back(pChannel);
        }
    }
}

void Epoller::addChannel(Channel *pChannel)
{
    int fd = pChannel->getfd();
    struct epoll_event ev;
    ev.events = pChannel->getEvents();
    ev.data.ptr = pChannel;
    // {
    //     std::lock_guard<std::mutex> mut(mutex_);
    //     channelMap_[fd] = pChannel;
    // }
    channelMap_[fd] = pChannel;
   
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll add error");
        exit(1);
    }
}

void Epoller::removeChannel(Channel *pChannel)
{
    int fd = pChannel->getfd();
    struct epoll_event ev;
    ev.events = pChannel->getEvents();
    ev.data.ptr = pChannel;
    // {
    //     std::lock_guard<std::mutex> mut(mutex_);
    //     channelMap_.erase(fd);
    // }
    channelMap_.erase(fd);

    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev) == -1)
    {
        perror("epoll delete error");
        exit(1);
    }
}

void Epoller::updateChannel(Channel *pChannel)
{
    int fd = pChannel->getfd();
    struct epoll_event ev;
    ev.events = pChannel->getEvents();
    ev.data.ptr = pChannel;
    
    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        perror("epoll modify error");
        exit(1);
    }
}