//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_EVENT_LOOP_THREAD_H
#define REACTORSERVER_EVENT_LOOP_THREAD_H


#include "common.h"
#include "event_loop.h"

#include <string>
#include <pthread.h>
#include <unistd.h>


class event_loop_thread {
public:
    friend void* event_loop_thread_run(void*);

    event_loop_thread()
            : pSubEventLoop(nullptr)
            , subThreadID(nullptr)
            , handleConnectCount(0)
            , index(-1)
    {
        pthread_mutex_init(&this->mutex, nullptr);
        pthread_cond_init(&this->cond, nullptr);
    }

    /** Thread Name in ThreadPool format: SubThread-index */
    void set_subThreadName(std::string name)
    {
        this->subThreadName = std::move(name);
    }

    /** Must be called by MainThread */
    event_loop* event_loop_thread_start(int index);

    int get_connect_count() const
    {
        return this->handleConnectCount;
    }

    event_loop* get_subEventLoop()
    {
        return this->pSubEventLoop;
    }

    int get_subThreadIndex() const
    {
        return this->index;
    }

private:
    event_loop* pSubEventLoop;

    pthread_t subThreadID;
    std::string subThreadName;

    int handleConnectCount;

    /** the position in ThreadPool */
    int index;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void* event_loop_thread_run(void*);

#endif //REACTORSERVER_EVENT_LOOP_THREAD_H