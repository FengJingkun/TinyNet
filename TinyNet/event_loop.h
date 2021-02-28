//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_EVENT_LOOP_H
#define REACTORSERVER_EVENT_LOOP_H

#include "common.h"

#include "event_dispatcher.h"
#include "poll_dispatcher.h"
#include "channel.h"
#include "log.h"

#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include <utility>
#include <cassert>


class event_loop {
public:
    static event_loop* event_loop_init(std::string&& name = "");

    int event_loop_run();

    static int channel_event_activate(void*, int, int);

    void event_loop_wakeup();
    static void handle_wakeup(void*);

    int add_channel_event(channel*);
    /** Only this func will release channel */
    int remove_channel_event(channel*);
    int update_channel_event(channel*);

    pthread_t get_threadId()
    {
        return this->threadId;
    }

    int get_SocketPair() const
    {
        return this->socketPair[1];
    }

    const char* get_loopName() const
    {
        return this->loopName.c_str();
    }

protected:
    event_loop(std::string&& name, event_dispatcher* handle):
            loopName(std::move(name)),
            isQuit(false),
            dispatcher(handle)
    {
        this->threadId = pthread_self();

        pthread_mutex_init(&this->mutex, nullptr);
        pthread_cond_init(&this->cond, nullptr);

        int res = socketpair(AF_UNIX, SOCK_STREAM, 0, this->socketPair);
        if (res < 0) {
            LOG_ERROR("EventLoop make SocketPair failed\n");
            exit(1);
        }
    }

    int do_channel_event(channel*, int);

    /** in the I/O Thread */
    int handle_pending_channel();

    int handle_pending_add(channel*);
    int handle_pending_remove(channel*);
    int handle_pending_update(channel*);

private:
    const std::string loopName;
    bool isQuit;

    event_dispatcher* dispatcher;

    std::unordered_map<int, channel*> channelMap;
    std::queue<channel_element*> pendingChannelList;

    pthread_t threadId;
    int socketPair[2];

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

#endif //REACTORSERVER_EVENT_LOOP_H