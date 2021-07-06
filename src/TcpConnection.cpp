#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "TcpConnection.h"
#include "../include/Utility.h"

const int BUFSIZE = 4096;

TcpConnection::TcpConnection(EventLoop *loop, int fd, const struct sockaddr_in &clientaddr)
    : loop_(loop),
      fd_(fd),
      connState_(ConnectionState::CONNECTED),
      clientaddr_(clientaddr),
      pChannel_(new Channel())
{
    pChannel_->setfd(fd_);
    pChannel_->setEvents(EPOLLIN | EPOLLET); // using Edge Trigger(ET), attention to the way of read() and write()
    pChannel_->setReadHandler(std::bind(&TcpConnection::handleRead, this));
    pChannel_->setWriteHandler(std::bind(&TcpConnection::handleWrite, this));
    pChannel_->setErrorhandler(std::bind(&TcpConnection::handleError, this));
    pChannel_->setCloseHandler(std::bind(&TcpConnection::handleClose, this));
}
TcpConnection::~TcpConnection()
{
    // remove event(channel) from eventloop, then deallocate the memory(automitically)
    close(fd_);
}

void TcpConnection::addChannelToLoop()
{
    loop_->runInLoop(std::bind(&EventLoop::addChannelToEpoller, loop_, pChannel_.get()));
}

void TcpConnection::handleRead()
{
    int ret = recvn(fd_, bufferin_);
    if (ret > 0)
    {
        messageCallback_(shared_from_this(), bufferin_);
    }
    else if (ret == 0) // clientfd closed the connection
        handleClose();
    else
        handleError();
}

void TcpConnection::handleWrite()
{
    int ret = sendn(fd_, bufferout_);
    if (ret >= 0)
    {
        uint32_t events = pChannel_->getEvents();
        if (bufferout_.length() > 0) // buffer is not sent completely
        {
            pChannel_->setEvents(events | EPOLLOUT);
            loop_->updateChannelToEpoller(pChannel_.get());
        }
        else // sent completed, remove EPOLLOUT
        {
            pChannel_->setEvents(events & (~EPOLLOUT));
            completeSendCallback_(shared_from_this());

            if (connState_ == ConnectionState::DISCONNECTING)
                handleClose();
        }
    }
    // else if (ret == 0)  //
    //     handleClose();
    else
        handleError();
}

void TcpConnection::handleError()
{
    bufferin_.clear();
    bufferout_.clear();
    handleClose();
}

void TcpConnection::handleClose()
{
    if (connState_ == ConnectionState::DISCONNECTED)
        return;
    
    if (!bufferin_.empty() || !bufferout_.empty()) 
    {
        connState_ = ConnectionState::DISCONNECTING;
        if (!bufferin_.empty()) // the server still need to process data
        {
            messageCallback_(shared_from_this(), bufferin_);
        }
    }
    else // cleanup the connection
    {
        loop_->runInLoop(std::bind(&EventLoop::removeChannelFromEpoller, loop_, pChannel_.get()));
        closeCallback_(shared_from_this());
        connState_ = ConnectionState::DISCONNECTED;
    }
}

void TcpConnection::setmessageHandler(const MessageCallback &cb)
{
    messageCallback_ = cb;
}

void TcpConnection::setCompletedSendHandler(const Callback &cb)
{
    completeSendCallback_ = cb;
}

void TcpConnection::setCloseHandler(const Callback &cb)
{
    closeCallback_ = cb;
}

void TcpConnection::setErrorHandler(const Callback &cb)
{
    errorCallback_ = cb;
}

void TcpConnection::setConnectionCleanupHandler(const Callback &cb)
{
    connectionCleanup_ = cb;
}

