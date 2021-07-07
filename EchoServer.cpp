#include "EchoServer.h"
#include <iostream>
#include <functional>

EchoServer::EchoServer(EventLoop* loop, const int port, const int iothreadnum, const int workthreadnum)
    : tcpserver_(loop, port, iothreadnum),
      threadpool_(workthreadnum)
{
    tcpserver_.setNewConnectionHandler(std::bind(&EchoServer::handleNewConnection, this, std::placeholders::_1));
    tcpserver_.setMessageHandler(std::bind(&EchoServer::handleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setCompletedSendHandler(std::bind(&EchoServer::handleSendComplete, this, std::placeholders::_1));
    tcpserver_.setCloseHandler(std::bind(&EchoServer::handleClose, this, std::placeholders::_1));
    tcpserver_.setErrorHandler(std::bind(&EchoServer::handleError, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

void EchoServer::start()
{
    threadpool_.start();
    tcpserver_.start();
}

void EchoServer::handleNewConnection(const spTcpConnection& sptcpconn)
{
    std::cout << "New Connection Come in" << std::endl;
}

void EchoServer::handleMessage(const spTcpConnection& sptcpconn, std::string &s)
{
    std::string msg;
    msg.swap(s);
    msg.insert(0, "reply msg:");
    sptcpconn->sendMsg(msg);
}

void EchoServer::handleSendComplete(const spTcpConnection& sptcpconn)
{
    std::cout << "recv a completed message!" <<std::endl;
}

void EchoServer::handleClose(const spTcpConnection& sptcpconn)
{
    std::cout << "EchoServer conn close" << std::endl;
}

void EchoServer::handleError(const spTcpConnection& sptcpconn)
{
    std::cout << "EchoServer error" << std::endl;
}