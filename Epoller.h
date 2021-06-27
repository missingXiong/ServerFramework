#include <iostream>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "Channel.h"

// a class for epoll
class Epoller
{
public:
    typedef std::vector<Channel*> ChannelList;

    Epoller();
    ~Epoller();

    void poll(ChannelList& activeChannelList); // equal to epoll_wait
    void addChannel(Channel* pChannel);
    void removeChannel(Channel* pChannel);
    void updateChannel(Channel* pChannel);

private:
    int epollfd_; // epoll fd
    std::vector<struct epoll_event> eventList_; // receive events from epoll_wait
    std::mutex mutex_; // mutex_ for channelMap_ because main thread will add new connection into eventLoop
    std::unordered_map<int, Channel*> channelMap_; // fd -> Channel
};