//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_ACCEPTOR_H
#define REACTORSERVER_ACCEPTOR_H


#include "common.h"
#include "log.h"

class acceptor {
public:
    static acceptor* acceptor_init(int port)
    {
        static auto* pAcceptor = new acceptor(port);
        pAcceptor->init();
        return pAcceptor;
    }

    int get_listen_fd() const
    {
        return this->listen_fd;
    }

    int get_listen_port() const
    {
        return this->listen_port;
    }

protected:
    explicit acceptor(int port): listen_fd(-1), listen_port(port) { }

    void init() {
        listen_fd = socket(PF_INET, SOCK_STREAM, 0);
        this->make_non_blocking();

        struct sockaddr_in serv_addr{};
        bzero(&serv_addr, sizeof serv_addr);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(this->listen_port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        socklen_t serv_len = sizeof serv_addr;

        int on = 1;
        setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

        int res = bind(this->listen_fd, (struct sockaddr *) &serv_addr, serv_len);
        if (res < 0) {
            LOG_ERROR("Acceptor: bind failed, Fd = %d\n", this->listen_fd);
            exit(-1);
        }

        res = listen(this->listen_fd, 1024);
        if (res < 0) {
            LOG_ERROR("Acceptor: Listen failed, Fd = %d, Port = %d\n",
                      this->listen_fd, this->listen_port);
            exit(-1);
        }

        LOG_MSG("Acceptor: start listening, Listen fd = %d, Port = %d\n",
                this->listen_fd, this->listen_port);
    }

private:
    void make_non_blocking() const
    {
        fcntl(this->listen_fd, F_SETFL, O_NONBLOCK);
    }

    int listen_fd;
    int listen_port;
};


#endif //REACTORSERVER_ACCEPTOR_H
