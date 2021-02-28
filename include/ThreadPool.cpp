//
// Created by ErMo on 2021/2/8.
//

#include <cassert>
#include "ThreadPool.h"

void ThreadPool::Initialize(EventLoop* eventLoop, int threadNumber)
{
    mainLoop = eventLoop;
    position = 0;
    threadNum = threadNumber;
    isStarted = false;
    eventLoopThreads = nullptr;
}

/** must be called by the main thread */
void ThreadPool::Start()
{
    assert(!isStarted);
    assert(mainLoop->ownerThreadId == pthread_self());

    isStarted = true;
    if (threadNum <= 0)
        return;

    eventLoopThreads = new EventLoopThread[threadNum];
    for (int i = 0; i < threadNum; ++i) {
        eventLoopThreads[i].Initialize(i);
        eventLoopThreads[i].Start();
    }
}

/** must be called by the main thread*/
EventLoop * ThreadPool::getLoop()
{
    assert(isStarted);
    assert(mainLoop->ownerThreadId == pthread_self());

    EventLoop* selected = mainLoop;
    if (threadNum > 0) {
        selected = eventLoopThreads[position].eventLoop;
        position = (position + 1) % threadNum;
    }
    return selected;
}