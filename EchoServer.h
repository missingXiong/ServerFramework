#include <string>
#include "include/TcpServer.h"
#include "include/EventLoop.h"
#include "include/TcpConnection.h"
#include "include/ThreadPool.h"

class EchoServer
{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;
    typedef std::function<void()> Task;

    EchoServer(EventLoop* loop, const int port, const int iothreadnum, const int workthreadnum);
    ~EchoServer();

    void start();

private:
    void handleNewConnection(const spTcpConnection& sptcpconn);
    void handleMessage(const spTcpConnection &sptcpconn, std::string &s);
    void handleSendComplete(const spTcpConnection& sptcpconn);
    void handleClose(const spTcpConnection& sptcpconn);
    void handleError(const spTcpConnection& sptcpconn);

    TcpServer tcpserver_;
    ThreadPool<Task> threadpool_;
};