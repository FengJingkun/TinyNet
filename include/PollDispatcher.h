//
// Created by ErMo on 2021/2/7.
//

#ifndef TINYNET_POLLDISPATCHER_H
#define TINYNET_POLLDISPATCHER_H

#include "common.h"
#include "EventDispatcher.h"
#include "EventLoop.h"

struct PollDispatcherData {
    int event_count;
    int nfds;
    struct pollfd* event_set;
};

class PollDispatcher final: public EventDispatcher {
public:
    PollDispatcher() = default;

    void* Init(EventLoop *) override;

    int Add(EventLoop *, Channel *) override;

    int Del(EventLoop *, Channel *) override;

    int Update(EventLoop *, Channel *) override;

    int Dispatch(EventLoop *, struct timeval *) override;

    void Clear(EventLoop *) override;
};


#endif //TINYNET_POLLDISPATCHER_H
