#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// a class for listenning socket
class ListenningSocket
{
public:
	ListenningSocket();
	~ListenningSocket();

	int fd() const { return serverfd_; }

	void setReuseAddr();
	void setNonBlocking();
	bool bindAddress(int port);
	bool listenning();
	bool acceptfd(struct sockaddr_in& clientaddr);

private:
	int serverfd_;
};