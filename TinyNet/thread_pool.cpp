//
// Created by ErMo on 2021/2/28.
//

#include "thread_pool.h"


void thread_pool::thread_loop_start()
{
    assert(this->isStarted == false);
    assert(this->pMainLoop->get_threadId() == pthread_self());
    this->isStarted = true;
    if (this->threadNum <= 0)
        return;

    this->pEventLoopThreads = new event_loop_thread[this->threadNum];

    for (int i = 0; i < this->threadNum; ++i) {
        std::string name = "SubThread-" + std::to_string(i + 1);
        this->pEventLoopThreads[i].set_subThreadName(std::move(name));

        this->pEventLoopThreads[i].event_loop_thread_start(i + 1);
    }
}


event_loop* thread_pool::get_sub_event_loop()
{
    assert(this->isStarted == true);
    assert(this->pMainLoop->get_threadId() == pthread_self());

    event_loop* selected = this->pMainLoop;
    if (this->threadNum > 0) {
        selected = this->pEventLoopThreads[this->position].get_subEventLoop();
        this->position = (this->position + 1) % this->threadNum;
    }
    return selected;
}