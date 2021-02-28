//
// Created by ErMo on 2021/2/28.
//

#include "tcp_server.h"


tcp_server* tcp_server::get_instance(event_loop* pEventLoop,
                                     acceptor* pAcceptor,
                                     int threadN)
{
    static auto* pTCPServer = new tcp_server(pEventLoop, pAcceptor, threadN);
    return pTCPServer;
}

int tcp_server::handle_connection_established(tcp_server* pServer)
{
    acceptor* pAcceptor = pServer->get_Acceptor();
    int listenFd = pAcceptor->get_listen_fd();

    struct sockaddr_in client_addr{};
    bzero(&client_addr, sizeof client_addr);
    socklen_t client_len;
    int connFd = accept(listenFd, (struct sockaddr *) &client_addr, &client_len);
    LOG_MSG("Acceptor: New Connect accepted, connfd = %d", connFd);

    /** make connFd non-blocking */
    fcntl(connFd, F_SETFL, O_NONBLOCK);

    /** Dispatch the new connection channel to the SubEventLoop from ThreadPool */
    event_loop* pSubEventLoop = pServer->get_ThreadPool()->get_sub_event_loop();

    /** Create a new connection instance */
    auto* pConnection = new tcp_connection();

    auto connectionAcceptedCallback = std::bind(onConnectionAccepted, pConnection);
    auto messageCallback = std::bind(onMessage, pConnection);
    auto writeCompletedCallback = std::bind(onWriteCompleted, pConnection);
    auto connectionClosedCallback = std::bind(onConnectionClosed, pConnection);

    pConnection->init(connFd, pSubEventLoop,
                      std::move(connectionAcceptedCallback),
                      std::move(messageCallback),
                      std::move(writeCompletedCallback),
                      std::move(connectionClosedCallback));

    return 0;
}


void tcp_server::start()
{
    this->pThreadPool->thread_loop_start();

    /** Register listen channel to MainLoop */
    channel* pChannel = channel::channel_new(this->pAcceptor->get_listen_fd(),
                                             EventType::EventRead);

    auto func = std::bind(&tcp_server::handle_connection_established, this);
    pChannel->set_read_callback(func);

    this->pMainLoop->add_channel_event(pChannel);
    this->pMainLoop->event_loop_run();
}