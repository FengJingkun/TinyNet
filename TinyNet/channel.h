//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_CHANNEL_H
#define REACTORSERVER_CHANNEL_H


#include "common.h"
#include <memory>
#include <functional>

class channel {
public:

    static channel* channel_new(int fd, int type)
    {
        auto* pChannel = new channel(fd, type);
        return pChannel;
    }

    void set_read_callback(channel_read_callback cb)
    {
        this->readCallback = std::move(cb);
    }

    void set_write_callback(channel_write_callback cb)
    {
        this->writeCallback = std::move(cb);
    }

    void channel_read()
    {
        this->readCallback();
    }

    void channel_write()
    {
        this->writeCallback();
    }

    void channel_write_enable()
    {
        this->eventType |= EventType::EventWrite;
    }

    void channel_write_disable()
    {
        this->eventType &= ~EventType::EventWrite;
    }

    int is_write_enabled() const
    {
        return this->eventType & EventType::EventWrite;
    }

    int get_channel_fd() const
    {
        return this->eventFd;
    }

    int get_channel_type() const
    {
        return this->eventType;
    }

    static void channel_free(channel* ptr)
    {
        ptr->eventFd = -1;
        ptr->eventType = -1;
        ptr->readCallback = nullptr;
        ptr->writeCallback = nullptr;
        delete ptr;
        ptr = nullptr;
    }

    channel() = delete;
    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;

protected:
    channel(int fd, int type):
            eventFd(fd),
            eventType(type),
            readCallback(nullptr),
            writeCallback(nullptr) { }

private:
    int eventFd;
    int eventType;

    channel_read_callback readCallback;
    channel_write_callback writeCallback;
};

struct channel_element {
    int operation;
    channel* element;

    channel_element(): operation(-1), element(nullptr) {}

    static void destroy(channel_element* ptr)
    {
        ptr->operation = -1;
        ptr->element = nullptr;
        delete ptr;
    }
};

#endif //REACTORSERVER_CHANNEL_H