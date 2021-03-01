//
// Created by ErMo on 2021/3/1.
//

#include "utils.h"


event_dispatcher* make_dispatcher(DispatcherType mode)
{
    event_dispatcher* pDispatcher = nullptr;
    switch (mode) {
        case DispatcherType::Poll:
            pDispatcher = new poll_dispatcher("Poll");
            break;
        case DispatcherType::Epoll:
            pDispatcher = new epoll_dispatcher("Epoll");
            break;
        default:
            pDispatcher = new poll_dispatcher("Poll");
            break;
    }
    return pDispatcher;
}


void make_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}