#include <sys/epoll.h>
#include <unistd.h>
#include "TcpConnection.h"

const int BUFSIZE = 4096;

TcpConnection::TcpConnection(EventLoop *loop, int fd, const struct sockaddr_in &clientaddr)
    : loop_(loop),
      fd_(fd),
      clientaddr_(clientaddr),
      pChannel_(new Channel())
{
    pChannel_->setfd(fd_);
    pChannel_->setEvents(EPOLLIN | EPOLLET); // using Edge Trigger(ET)
    pChannel_->setReadHandler(std::bind(&TcpConnection::handleRead, this));
    pChannel_->setWriteHandler(std::bind(&TcpConnection::handleWrite, this));
    pChannel_->setErrorhandler(std::bind(&TcpConnection::handleError, this));
    pChannel_->setCloseHandler(std::bind(&TcpConnection::handleClose, this));
}
TcpConnection::~TcpConnection()
{
    // remove event(channel) from eventloop, then deallocate the memory(automitically)
    loop_->addPendingTask(std::bind(&EventLoop::removeChannelFromEpoller, loop_, pChannel_.get()));
    close(fd_);
}

void TcpConnection::addChannelToLoop()
{
}
void TcpConnection::send(const std::string &)
{
}
void TcpConnection::sendInLoop()
{
}
void TcpConnection::shutdown()
{
}
void TcpConnection::shutdownInLoop()
{
}

void TcpConnection::handleRead()
{
    
}
void TcpConnection::handleWrite()
{
}
void TcpConnection::handleError()
{
}
void TcpConnection::handleClose()
{
}

void TcpConnection::setmessageHandler(const MessageCallback &cb)
{
}
void TcpConnection::setCompletedSendHandler(const Callback &cb)
{
}
void TcpConnection::setCloseHandler(const Callback &cb)
{
}
void TcpConnection::setErrorHandler(const Callback &cb)
{
}
void TcpConnection::setConnectionCleanupHandler(const Callback &cb)
{
}

int TcpConnection::sendn(int fd, std::string &bufferout)
{
    size_t nleft = bufferout.size();
    ssize_t nbyte = 0;
    ssize_t writeSum = 0;
    const char *buf = bufferout.c_str();
    while (nleft > 0)
    {
        nbyte = write(fd, buf, nleft);
        if (nbyte > 0)
        {
            nleft -= nbyte;
            writeSum += nbyte;
            buf += nbyte;
        }
        else if (nbyte < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return writeSum;
            else if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else 
            break;
    }

    return writeSum;
}

int TcpConnection::recvn(int fd, std::string &bufferin)
{
    int nbyte = 0;
    int readByte = 0;
    char buffer[BUFSIZE];

    while (true)
    {
        nbyte = read(fd, buffer, BUFSIZE);
        if (nbyte > 0)
        {
            bufferin.append(buffer, nbyte);
            readByte += nbyte;
            if (nbyte < BUFSIZE) // read completed
                return readByte;
            else
                continue;
        }
        else if (nbyte < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // data is not ready
                return readByte;
            else if (errno == EINTR) // interrupted, continue to read
                continue;
            else 
                return -1;
        }
        else 
            break;
    }

    return readByte;
}