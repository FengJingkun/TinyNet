//
// Created by ErMo on 2021/2/3.
//

#ifndef TINYNET_EVENTLOOP_H
#define TINYNET_EVENTLOOP_H

#include "common.h"
#include "EventDispatcher.h"
#include "Channel.h"
#include "ChannelMap.h"

enum EventType: int {
    Timeout  = 0x01,
    READ = 0x02,
    WRITE = 0x04,
    Signal   = 0x08
};

struct EventLoop {
    int quit;
    EventDispatcher* eventDispatcher;
    void* eventDispatcherData;

    ChannelMap* channelMap;

    int isHandlePending;
    ChannelElement* pendingHead;
    ChannelElement* pendingTail;

    pthread_t ownerThreadId;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int socketPair[2];
    char* threadName;
};

EventLoop* EventLoopInit (char* = nullptr);

int EventLoopRun (EventLoop*);

int HandlePendingChannels (EventLoop*);

int AddPendingChannel (EventLoop*, Channel*);

int DelPendingChannel (EventLoop*, Channel*);

int UpdatePendingChannel (EventLoop*, Channel*);


void WakeupSubEventLoop (EventLoop*);

// dispatcher分发完事件后，调用该方法通知eventLoop执行相关callback
int ActivateChannel (EventLoop*, int, int);

int PushChannel (EventLoop*, Channel*);

int PopChannel (EventLoop*, Channel*);

int ModChannel (EventLoop*, Channel*);

#endif //TINYNET_EVENTLOOP_H
