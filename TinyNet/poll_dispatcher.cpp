//
// Created by ErMo on 2021/2/27.
//

#include "poll_dispatcher.h"


void poll_dispatcher_data::init()
{
    this->pEventSet = new pollfd[1024];
    for (int i = 0; i < 1024; ++i)
        this->pEventSet[i].fd = -1;
}


void poll_dispatcher::set_callback(channel_activate_callback cb)
{
    this->activate_channel = std::move(cb);
}

int poll_dispatcher::dispatch()
{
    int n = poll(this->data->pEventSet, 1024, 0);
    if (n < 0) {
        printf("poll failed\n");
        return -1;
    }
    if (n == 0) return 0;

    for (int i = 0; i < 1024; ++i) {
        if (this->data->pEventSet[i].fd == -1) continue;

        struct pollfd cur = this->data->pEventSet[i];
        if (cur.revents > 0) {
            printf("get message channel = %d, %s\n", cur.fd, this->dispatcherName.c_str());

            if (cur.revents & POLLRDNORM)
                this->activate_channel(cur.fd, EventType::EventRead);
            if (cur.revents & POLLWRNORM)
                this->activate_channel(cur.fd, EventType::EventWrite);
        }
        if (--n <= 0) break;
    }
    return 0;
}

int poll_dispatcher::add(int fd, int events)
{
    short pollEvents = 0;
    if (events & EventType::EventRead)
        pollEvents |= POLLRDNORM;
    if (events & EventType::EventWrite)
        pollEvents |= POLLWRNORM;

    int i = 0;
    for (; i < 1024; ++i) {
        if (this->data->pEventSet[i].fd == -1) {
            this->data->pEventSet[i].fd = fd;
            this->data->pEventSet[i].events = pollEvents;
            break;
        }
    }
    return i >= 1024? -1: 0;
}

int poll_dispatcher::del(int fd, int events)
{
    int i = 0;
    for (; i < 1024; ++i) {
        if (this->data->pEventSet[i].fd == fd) {
            this->data->pEventSet[i].fd = -1;
            break;
        }
    }
    return i >= 1024? -1: 0;
}

int poll_dispatcher::update(int fd, int events)
{
    short pollEvents = 0;
    if (events & EventType::EventRead)
        pollEvents |= POLLRDNORM;
    if (events & EventType::EventWrite)
        pollEvents |= POLLWRNORM;

    int i = 0;
    for (; i < 1024; ++i) {
        if (this->data->pEventSet[i].fd == fd) {
            this->data->pEventSet[i].events = pollEvents;
            break;
        }
    }
    return i >= 1024? -1: 0;
}