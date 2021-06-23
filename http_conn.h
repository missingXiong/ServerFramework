#pragma once
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>

#include "locker.h"

class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;

    static int m_epollfd; 
    static int m_user_count; // user count

    // header methods
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTION,
        CONNECT,
        PATCH
    };

    // parse states
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    // process reslut
    enum HTTP_CODE
    {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_REQUEST,
        CLOSED_CONNECTION
    };

    // request line status
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    http_conn(/* args */);
    ~http_conn();

    // new connection
    void init(int sockfd, const sockaddr_in &addr);
    // close connection
    void close_conn(bool real_close = true);
    // process request
    void process();
    // nonblocking read
    void read();
    // nonblocking write
    void write();

private:
    int m_sockfd; // every http_conn instance regulate a sockfd;
    sockaddr_in m_address; // client address
    
    char m_read_buf[READ_BUFFER_SIZE];
    int m_read_idx;
    int m_check_idx;
    int m_start_line;
    char m_write_buf[WRITE_BUFFER_SIZE];
    int m_write_idx;
    CHECK_STATE m_check_state;
    METHOD m_method;
    char m_real_file[FILENAME_LEN]; // url + doc root
    char* m_url;
    char* m_http_version; // http version
    char* m_host; // host name
    int m_content_length; // content length
    bool m_linger; // keep alive
    char* m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;

private:
    // initialize connection
    void init();
    
    // parse http request
    HTTP_CODE process_read();
    HTTP_CODE parse_request_line(char* text);
    HTTP_CODE parse_headers(char* text);
    HTTP_CODE parse_content(char* text);
    HTTP_CODE do_request();
    char* get_line() { return m_read_buf + m_start_line; }
    LINE_STATUS parse_line();
    
    // response
    bool process_write(HTTP_CODE ret);
    void unmap();
    bool add_response(const char* format, ...);
    bool add_content(const char* content);
    bool add_status_line(int status, const char* title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();
};
