#include <iostream>
#include <vector>
#include <map>
#include <mutex>

#include "Channel.h"

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
    int epollfd_;
    std::vector<struct epoll_event> eventList_; // receive events from epoll_wait
    std::map<int, Channel*> channelMap_;
    std::mutex mutex_;
};