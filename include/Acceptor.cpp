//
// Created by ErMo on 2021/2/7.
//

#include "Acceptor.h"
#include <assert.h>

void Acceptor::Initialize(int port)
{
    mListenPort = port;
    mListenFd = socket(PF_INET, SOCK_STREAM, 0);

    // make listen fd non-blocking
    fcntl(mListenFd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in serv_addr{};
    bzero(&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mListenPort);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(mListenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

    socklen_t serv_len = sizeof serv_addr;
    int rt = bind(mListenFd, (struct sockaddr *) &serv_addr, serv_len);
    if (rt < 0)
        printf("bind failed");

    rt = listen(mListenFd, 1024);
    if (rt < 0)
        printf("listen failed");
}