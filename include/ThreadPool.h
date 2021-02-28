//
// Created by ErMo on 2021/2/8.
//

#ifndef TINYNET_THREADPOOL_H
#define TINYNET_THREADPOOL_H

#include "EventLoop.h"
#include "EventLoopThread.h"

class ThreadPool {
public:
    EventLoop* mainLoop;
    bool isStarted;
    int threadNum;
    EventLoopThread* eventLoopThreads;
    int position;

    void Initialize (EventLoop*, int);

    void Start();

    EventLoop* getLoop();
};


#endif //TINYNET_THREADPOOL_H
