#pragma once

#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "ListenningSocket.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

/*
1. mainLoop, responsible for new connection
2. store all tcp connections
3. commit new connection to eventLoop(IO thread)
*/

class TcpServer : public noncopyable
{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;
    typedef std::function<void(const spTcpConnection&, std::string&)> MessageCallback;
    typedef std::function<void(const spTcpConnection&)> Callback;

public:
    TcpServer(EventLoop* loop, const int port, const int threadnum = std::thread::hardware_concurrency());
    ~TcpServer();

    void start();

    void setNewConnectionHandler(const Callback& cb); 
    void setCloseHandler(const Callback& cb);
    void setErrorHandler(const Callback& cb);
    void setMessageHandler(const MessageCallback& cb);
    void setCompletedSendHandler(const Callback& cb);

private:
    void dispatchNewConnection(); // push the new connction into EventLoop using polling cycle strategy
    void removeConnection(const std::shared_ptr<TcpConnection> conn);
    void connectError();

    void createListenfd(const int port);
private:
    ListenningSocket listenfd_;
    int port_;
    int ioThreadNum_;

    std::mutex mutex_;
    bool started_;
    int connectionCount_;
    EventLoop* mainLoop_; // main loop: detect new connection
    EventLoopThreadPool ioThreadPool_;
    Channel acceptChannel_; 
    std::map<int, std::shared_ptr<TcpConnection>> tcpConnections_;
    
    Callback newConnectionCB_;
    Callback closeConnectionCB_;
    Callback errorConnectionCB_;
    Callback sendCompletedCB_;
    MessageCallback messageCB_;
};