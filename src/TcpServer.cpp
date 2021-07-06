#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "../include/TcpServer.h"
#include "../include/Utility.h"

const int MAXCONNECTION = 20000;

TcpServer::TcpServer(EventLoop* loop, const int port, const int threadnum)
    : connectionCount_(0),
      mainLoop_(loop),
      ioThreadNum_(threadnum),
      ioThreadPool_(loop, threadnum)
{
    // initialize listenfd
    createListenfd(port);

    // setup listenfd events and callbacks
    acceptChannel_.setfd(listenfd_.fd());
    acceptChannel_.setEvents(EPOLLIN | EPOLLET);
    acceptChannel_.setWriteHandler(std::bind(&TcpServer::dispatchNewConnection, this));
    acceptChannel_.setErrorhandler(std::bind(&TcpServer::connectError, this));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    ioThreadPool_.start();
    mainLoop_->addChannelToEpoller(&acceptChannel_);
}

void TcpServer::setNewConnectionHandler(const Callback &cb)
{
    newConnectionCB_ = cb;
}

void TcpServer::setCloseHandler(const Callback &cb)
{
    closeConnectionCB_ = cb;
}

void TcpServer::setErrorHandler(const Callback &cb)
{
    errorConnectionCB_ = cb;
}

void TcpServer::setMessageHandler(const MessageCallback &cb)
{
    messageCB_  = cb;
}

void TcpServer::setCompletedSendHandler(const Callback &cb)
{
    sendCompletedCB_ = cb;
}

// void TcpServer::createListenfd(const int port)
// {
//     struct sockaddr_in serveraddr;
// 	memset(&serveraddr, 0, sizeof(serveraddr));
// 	serveraddr.sin_family = AF_INET;
// 	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(_ServerIP.c_str());
// 	serveraddr.sin_port = htons(port);
//     listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
//     assert(listenfd_ >= 0);
    
//     int on = 1;
//     setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
// 	int ret = bind(listenfd_, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
//     assert(ret != -1);
//     ret = listen(listenfd_, 2048);
//     assert(ret != -1);
// }

void TcpServer::dispatchNewConnection() // accept new connection and push the new connction into EventLoop using polling cycle strategy
{
    struct sockaddr_in clientaddr;
    int clientfd;
    while (clientfd = listenfd_.acceptfd(clientaddr) > 0)
    {
        if (connectionCount_ >= MAXCONNECTION)
        {
            close(clientfd);
            return;
        }
        ++connectionCount_;
        setnonblocking(clientfd);

        EventLoop* nxtLoop = ioThreadPool_.getNextEventLoop();
        
        // setup new connection
        std::shared_ptr<TcpConnection> spTcpConn(new TcpConnection(nxtLoop, clientfd, clientaddr));
        spTcpConn->setmessageHandler(messageCB_);
        spTcpConn->setCompletedSendHandler(sendCompletedCB_);
        spTcpConn->setCloseHandler(closeConnectionCB_);
        spTcpConn->setErrorHandler(errorConnectionCB_);
        spTcpConn->setConnectionCleanupHandler(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
        {
            std::lock_guard<std::mutex> mut(mutex_);
            tcpConnections_[clientfd] = spTcpConn;
        }
        newConnectionCB_(spTcpConn); // notify application layer
        spTcpConn->addChannelToLoop(); // add channel to nxtLoop crossing thread
    }
}

void TcpServer::removeConnection(const std::shared_ptr<TcpConnection> conn)
{
    std::lock_guard<std::mutex> mut(mutex_);
    tcpConnections_.erase(conn->getfd());
}

void TcpServer::connectError()
{
    
}

void TcpServer::createListenfd(const int port)
{
    listenfd_.setReuseAddr();
    listenfd_.bindAddress(port);
    listenfd_.listenning(); 
    listenfd_.setNonBlocking();
}