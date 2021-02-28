//
// Created by ErMo on 2021/2/28.
//

#include "event_loop_thread.h"


void* event_loop_thread_run(void* handle)
{
    auto pEventLoopThread = static_cast<event_loop_thread*>(handle);
    pthread_mutex_lock(&pEventLoopThread->mutex);

    std::string name = "SubLoop-" + std::to_string(pEventLoopThread->get_subThreadIndex());
    pEventLoopThread->pSubEventLoop = event_loop::event_loop_init(std::move(name));

    pthread_cond_signal(&pEventLoopThread->cond);
    pthread_mutex_unlock(&pEventLoopThread->mutex);

    pEventLoopThread->pSubEventLoop->event_loop_run();
}


event_loop* event_loop_thread::event_loop_thread_start(int i)
{
    this->index = i;
    pthread_create(&this->subThreadID, nullptr, event_loop_thread_run, this);

    pthread_mutex_lock(&this->mutex);

    while (this->pSubEventLoop == nullptr)
        pthread_cond_wait(&this->cond, &this->mutex);

    pthread_mutex_unlock(&this->mutex);

    LOG_MSG("%s: Bind to Thread-%d, subEventLoop = %s\n",
            this->subThreadName.c_str(),
            this->subThreadID,
            this->pSubEventLoop->get_loopName());

    return this->pSubEventLoop;
}