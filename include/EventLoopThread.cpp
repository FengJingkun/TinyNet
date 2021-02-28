//
// Created by ErMo on 2021/2/8.
//

#include "EventLoopThread.h"
#include <assert.h>

int EventLoopThread::Initialize(int i)
{
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);

    eventLoop = nullptr;
    threadCount = 0;
    tid = 0;

    char* buf = new char[16];
    sprintf(buf, "Thread-%d", i + 1);
    threadName = buf;
    return 0;
}

void* EventLoopThreadRun(void* arg)
{
    auto* eventLoopThread = static_cast<EventLoopThread *>(arg);
    pthread_mutex_lock(&eventLoopThread->mutex);
    pthread_cond_signal(&eventLoopThread->cond);
    pthread_mutex_unlock(&eventLoopThread->mutex);

    EventLoopRun(eventLoopThread->eventLoop);
}

EventLoop * EventLoopThread::Start()
{
    pthread_create(&this->tid, nullptr, &EventLoopThreadRun, this);

    pthread_mutex_lock(&mutex);
    while (eventLoop == nullptr)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    return eventLoop;
}