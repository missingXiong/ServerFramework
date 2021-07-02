#pragma once

#include <string>

int sendn(int fd, std::string& bufferout); // return 0, send buffer is full
int recvn(int fd, std::string& bufferin); // return 0, client has disconnected the connection
int setnonblocking(int fd);