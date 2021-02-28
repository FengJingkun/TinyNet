//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_TCP_SERVER_H
#define REACTORSERVER_TCP_SERVER_H


#include "common.h"
#include "log.h"
#include "event_loop.h"
#include "acceptor.h"
#include "thread_pool.h"
#include "callbacks.h"
#include "tcp_connection.h"

/** tcp_server is a singleton class */
class tcp_server {
public:
    /** Return the tcp_server instance */
    static tcp_server* get_instance(event_loop*, acceptor*, int);

    /** Callback func for a new tcp connection accepted by Acceptor */
    static int handle_connection_established(tcp_server*);

    /** Start running All EventLoops */
    void start();

    event_loop* get_MainLoop()
    {
        return this->pMainLoop;
    }

    acceptor* get_Acceptor()
    {
        return this->pAcceptor;
    }

    thread_pool* get_ThreadPool()
    {
        return this->pThreadPool;
    }

private:
    tcp_server(event_loop* Loop, acceptor* Acceptor, int threadN)
            : pMainLoop(Loop)
            , pAcceptor(Acceptor)
            , threadNum(threadN)
            , pThreadPool(nullptr)
            , httpHandle(nullptr)
    {
        /** Bind MainLoop to ThreadPool */
        this->pThreadPool = thread_pool::get_instance(this->pMainLoop, this->threadNum);
    }

    event_loop* const pMainLoop;
    acceptor* const pAcceptor;

    int threadNum;
    thread_pool* pThreadPool;

    void* httpHandle;
};

#endif //REACTORSERVER_TCP_SERVER_H