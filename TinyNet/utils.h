//
// Created by ErMo on 2021/3/1.
//

#ifndef REACTORSERVER_UTILS_H
#define REACTORSERVER_UTILS_H

#include "common.h"
#include "event_dispatcher.h"
#include "poll_dispatcher.h"
#include "epoll_dispatcher.h"

#include <string>


event_dispatcher* make_dispatcher(DispatcherType mode);


void make_nonblocking(int);


#endif //REACTORSERVER_UTILS_H
