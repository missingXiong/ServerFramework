#pragma once


class HttpServer // http application, users need to setup TcpServer
{
public:
    HttpServer();
    ~HttpServer();

    void onMessage();
};