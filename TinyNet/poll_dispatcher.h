//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_POLL_DISPATCHER_H
#define REACTORSERVER_POLL_DISPATCHER_H

#include "common.h"
#include "event_dispatcher.h"


class poll_dispatcher_data: public dispatcher_data {
public:

    poll_dispatcher_data(): dispatcher_data(), pEventSet(nullptr) { }

    int handleEventCount() const
    {
        return this->handleEventCounts;
    };

    void init() override;

    struct pollfd* pEventSet;
};


class poll_dispatcher: public event_dispatcher {
public:
    explicit poll_dispatcher(std::string&& name)
        : event_dispatcher(std::move(name))
        , data(nullptr)
    {
        data = new poll_dispatcher_data();
        data->init();
    }

    void set_callback(channel_activate_callback) override;

    int dispatch() override;

    int add(int fd, int events) override;

    int del(int fd, int events) override;

    int update(int fd, int events) override;

    char* get_dispatcher_name() const
    {
        return const_cast<char*>(this->dispatcherName.c_str());
    }

private:
    poll_dispatcher_data* data;
};

#endif //REACTORSERVER_POLL_DISPATCHER_H