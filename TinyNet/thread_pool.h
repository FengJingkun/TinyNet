//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_THREAD_POOL_H
#define REACTORSERVER_THREAD_POOL_H


#include "common.h"
#include "event_loop_thread.h"

/** Singleton Class **/
class thread_pool {
public:
    static thread_pool* get_instance(event_loop* mainLoop, int threadN)
    {
        static auto* pThreadPool = new thread_pool(mainLoop, threadN);
        return pThreadPool;
    }

    /** must be called by MainThread */
    void thread_loop_start();

    /** return a SubEventLoop from ThreadPool */
    event_loop* get_sub_event_loop();

private:
    thread_pool(event_loop* mainLoop, int threadN)
            : pMainLoop(mainLoop)
            , threadNum(threadN)
            , isStarted(false)
            , pEventLoopThreads(nullptr)
            , position(0) { }

    event_loop* pMainLoop;
    int threadNum;
    bool isStarted;
    event_loop_thread* pEventLoopThreads;
    int position;
};

#endif //REACTORSERVER_THREAD_POOL_H
