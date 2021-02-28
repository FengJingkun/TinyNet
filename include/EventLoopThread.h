//
// Created by ErMo on 2021/2/8.
//

#ifndef TINYNET_EVENTLOOPTHREAD_H
#define TINYNET_EVENTLOOPTHREAD_H

#include "EventLoop.h"


class EventLoopThread {
public:
    EventLoop* eventLoop;
    pthread_t tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char* threadName;
    long threadCount;

    int Initialize(int);

    EventLoop* Start();
};

#endif //TINYNET_EVENTLOOPTHREAD_H
