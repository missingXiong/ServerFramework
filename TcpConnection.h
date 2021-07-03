#pragma once

#include  <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <memory>
#include <string>

#include "EventLoop.h"
#include "Channel.h"

typedef enum
{
    CONNECTED,
    DISCONNECTING, // after send completed, the state will be set to DISCONNECTED
    DISCONNECTED // ready to close fd
} ConnectionState;

class TcpConnection: public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;
    typedef std::function<void(const spTcpConnection&)> Callback;
    typedef std::function<void(const spTcpConnection&, std::string&)> MessageCallback;

    TcpConnection(EventLoop* loop, int fd, const struct sockaddr_in& clientaddr);
    ~TcpConnection();

    int getfd() const { return fd_; }

    EventLoop* getLoop() const { return loop_; }

    void addChannelToLoop();

    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();

    void setmessageHandler(const MessageCallback& cb);
    void setCompletedSendHandler(const Callback& cb);
    void setCloseHandler(const Callback& cb);
    void setErrorHandler(const Callback& cb);
    void setConnectionCleanupHandler(const Callback& cb);

private:
    EventLoop* loop_;  // the current connection is registered in EventLoop loop_;
    int fd_;
    ConnectionState connState_;
    struct sockaddr_in clientaddr_;
    std::unique_ptr<Channel> pChannel_;
    
    // buffer
    std::string bufferin_;
    std::string bufferout_;

    // callbacks
    MessageCallback messageCallback_;
    Callback completeSendCallback_;
    Callback closeCallback_;
    Callback errorCallback_;
    Callback connectionCleanup_;
};