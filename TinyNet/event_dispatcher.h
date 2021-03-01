//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_EVENT_DISPATCHER_H
#define REACTORSERVER_EVENT_DISPATCHER_H


#include "common.h"

#include <string>
#include <utility>


enum DispatcherType: int {
    Poll = 1,
    Epoll = 2
};


class dispatcher_data {
public:
    dispatcher_data(): handleEventCounts(0) {};

    virtual void init() = 0;

    int handleEventCounts;
};


class event_dispatcher {
public:
    explicit event_dispatcher(std::string&& name): dispatcherName(std::move(name)) {};
    virtual void set_callback(channel_activate_callback) = 0;

    virtual int dispatch() = 0;

    virtual int add(int, int) = 0;
    virtual int del(int, int) = 0;
    virtual int update(int, int) = 0;

protected:
    const std::string dispatcherName;
    channel_activate_callback activate_channel;
};

event_dispatcher* make_dispatcher(std::string);

#endif //REACTORSERVER_EVENT_DISPATCHER_H