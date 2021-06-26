#pragma once
#include <iostream>
#include <functional>

// a class for events: different events have different callback
class Channel
{
    typedef std::function<void()> Callback;

    Channel() : fd_(-1) { }
    ~Channel() { }

    void setfd(int fd) {fd_ = fd;}
    int getfd() const { return fd_; }

    void setEvents(uint32_t events) { events_ = events; }
    uint32_t getEvents() const { return events_; }

    void setReadHandler(const Callback& cb) { readHandler_ = cb; }
    void setWriteHandler(const Callback& cb) { writeHandler_ = cb; }
    void setErrorhandler(const Callback& cb) { errorHandler_ = cb; }
    void setCloseHandler(const Callback& cb) { closeHandler_ = cb; }

    void handleEvents();

private:
    int fd_; //  Channel is no the owner!
    uint32_t events_;

    Callback readHandler_;
    Callback writeHandler_;
    Callback errorHandler_;
    Callback closeHandler_;
};