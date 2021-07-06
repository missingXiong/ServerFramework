#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../include/Utility.h"

const int BUFSIZE = 4096; // minimum buffer size, can be set bigger

int sendn(int fd, std::string &bufferout)
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
            else // error
                return -1;
        }
        else // nbyte == 0, send buffer is full
            break;
    }

    bufferout = bufferout.substr(writeSum);
    return writeSum;
}

int recvn(int fd, std::string &bufferin)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[BUFSIZE];
        memset(buff, '\0', BUFSIZE);
        nread = read(fd, buff, BUFSIZE);
        if (nread < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return readSum;
            else if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else if (nread == 0)
            break;

        readSum += nread;
        bufferin += std::string(buff, nread);
    }
    return readSum;
}

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}