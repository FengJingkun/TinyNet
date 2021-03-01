//
// Created by ErMo on 2021/3/1.
//

#ifndef REACTORSERVER_EPOLL_DISPATCHER_H
#define REACTORSERVER_EPOLL_DISPATCHER_H

#include "event_dispatcher.h"
#include <string>


class epoll_dispatcher_data: public dispatcher_data {

};


class epoll_dispatcher: public event_dispatcher {
public:
    explicit epoll_dispatcher(std::string&& name)
        : event_dispatcher(std::move(name))
        , data(nullptr)
    {}

private:
    epoll_dispatcher_data* data;
};

#endif //REACTORSERVER_EPOLL_DISPATCHER_H
