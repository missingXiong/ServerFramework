#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class ListeningSocket
{
public:
	ListeningSocket();
	~ListeningSocket();

	int fd() const { return fd_; }
};