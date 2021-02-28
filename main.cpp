#include <iostream>

#include "TinyNet/log.h"
#include "TinyNet/acceptor.h"
#include "TinyNet/event_loop.h"
#include "TinyNet/tcp_server.h"

int main() {
    LOG_MSG("main thread = %d\n", pthread_self());

    acceptor* pAcceptor = acceptor::acceptor_init(12345);
    event_loop* pEventLoop = event_loop::event_loop_init();

    tcp_server* pServer = tcp_server::get_instance(pEventLoop, pAcceptor, 4);
    pServer->start();

    return 0;
}