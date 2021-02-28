//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_COMMON_H
#define REACTORSERVER_COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include "sys/uio.h"
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include <signal.h>
#include <sys/select.h>
#include <poll.h>

#include <pthread.h>

#include <memory>
#include <functional>

#include "log.h"

enum ChannelOpt: int {
    ADD = 0,
    DEL,
    MOD
};

enum EventType: int {
    EventTimeout = 0x01,
    /** Wait for a fd to become readable */
    EventRead = 0x02,
    /** Wait for a fd to become writeable */
    EventWrite = 0x04,
    /** Wait for a signal to be raised*/
    EventSignal = 0x08
};


typedef std::function<int(int, int)> channel_activate_callback;
typedef std::function<void(void)> channel_read_callback;
typedef std::function<void(void)> channel_write_callback;

typedef std::function<void(void)> connection_accepted_callback;
typedef std::function<int(void)> message_process_callback;
typedef std::function<void(void)> write_completed_callback;
typedef std::function<void(void)> connection_closed_callback;



/** data process **/
//char rot13_char(char c) {
//    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
//        return c + 13;
//    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
//        return c - 13;
//    else
//        return c;
//}

#endif //REACTORSERVER_COMMON_H