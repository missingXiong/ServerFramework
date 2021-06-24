#include <fcntl.h>
#include "http_conn.h"

const char *ok_200_title = "OK";
const char *error_400_title = "Rad Request";
const char *error_400_content = "Your request has bad syntax";
const char *error_403_title = "Forbidden";
const char *error_403_content = "You don't have permission to get file from this server";
const char *error_404_title = "Not Found";
const char *error_404_content = "The request was not found on the server";
const char *error_500_title = "Internal Server Error";
const char *error_500_content = "There is an error on the server";

void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

// By default: one_shot == true to avoid multiple threads process the same socket
void addfd(int epollfd, int fd, bool one_shot = true)
{
    epoll_event event;
    event.data.fd = fd;
    /* using epoll as an ET interface:
        by waiting for an event only after read or write return EAGAIN
    */
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

// initialize static data
int http_conn::m_epollfd = -1;
int http_conn::m_user_count = 0;

http_conn::http_conn(/* args */)
{
}

http_conn::~http_conn()
{
}

void http_conn::init(int sockfd, const sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr;

    int reuse = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    addfd(m_epollfd, m_sockfd, true);
    m_user_count++;
    init();
}

void http_conn::close_conn(bool real_close)
{
}

void http_conn::process()
{
}

void http_conn::read()
{
}

void http_conn::write()
{
}

void http_conn::init()
{
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_linger = false;
    m_method = METHOD::GET;
    m_url = NULL;
    m_http_version = NULL;
    m_content_length = 0;
    m_host = NULL;
    m_start_line = 0;
    m_check_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

// parse http request
http_conn::HTTP_CODE http_conn::process_read()
{
}

http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{
}
http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{
}

http_conn::HTTP_CODE http_conn::parse_content(char *text)
{
}
http_conn::HTTP_CODE http_conn::do_request()
{
}

http_conn::LINE_STATUS http_conn::parse_line()
{
}

bool http_conn::process_write(http_conn::HTTP_CODE ret)
{
}

void http_conn::unmap()
{
}

bool http_conn::add_response(const char *format, ...)
{
}

bool http_conn::add_content(const char *content)
{
}

bool http_conn::add_status_line(int status, const char *title)
{
}

bool http_conn::add_headers(int content_length)
{
}

bool http_conn::add_content_length(int content_length)
{
}

bool http_conn::add_linger()
{
}

bool http_conn::add_blank_line()
{
}