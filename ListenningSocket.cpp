#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>
#include <fcntl.h>

#include "ListenningSocket.h"

ListenningSocket::ListenningSocket()
{
    serverfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd_ == -1)
    {
        perror("Socket create failure");
        exit(-1);
    }
}

ListenningSocket::~ListenningSocket()
{
    close(serverfd_);
}

void ListenningSocket::setReuseAddr()
{

}
void ListenningSocket::setNonBlocking()
{
    fcntl(serverfd_, F_SETFL, fcntl(serverfd_, F_GETFL) | O_NONBLOCK);
}

bool ListenningSocket::bindAddress(int port)
{
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    int ret = bind(serverfd_, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == -1)
    {
        close(serverfd_);
        perror("bind error");
        exit(1);
    }
    return true;
}

bool ListenningSocket::listenfd()
{
    if (listen(serverfd_, 8192) < 0)
    {
        close(serverfd_);
        perror("listen error");
        exit(1);
    }
    return true;
}

bool ListenningSocket::acceptfd(struct sockaddr_in& clientaddr)
{
    socklen_t length = (socklen_t)sizeof(clientaddr);
    int fd = accept(serverfd_, (struct sockaddr*)&clientaddr, &length);
    if (fd < 0)
    {
        if (errno == EAGAIN)
            return 0;
        return fd;
    }
    return fd;
}