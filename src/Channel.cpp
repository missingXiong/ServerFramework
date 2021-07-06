#include <sys/epoll.h>

#include "../include/Channel.h"

void Channel::handleEvents()
{
    if (events_ & EPOLLRDHUP)
        closeHandler_();
    else if (events_ & EPOLLIN)
        readHandler_();
    else if (events_ & EPOLLOUT)
        writeHandler_();
    else if (events_ & EPOLLERR)
        errorHandler_();
}