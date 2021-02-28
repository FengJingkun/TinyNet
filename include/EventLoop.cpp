//
// Created by ErMo on 2021/2/3.
//
#include <assert.h>

#include "EventLoop.h"
#include "common.h"
#include "Channel.h"
#include "EventDispatcher.h"

int HandlePendingChannels(EventLoop* eventLoop)
{
    pthread_mutex_lock(&eventLoop->mutex);
    eventLoop->isHandlePending = 1;
    ChannelElement* channelElement = eventLoop->pendingHead;
    while (channelElement) {
        Channel* channel = channelElement->channel;
        switch (channelElement->operationType) {
            case 1:
                AddPendingChannel(eventLoop, channel);
                break;
            case 2:
                DelPendingChannel(eventLoop, channel);
                break;
            case 3:
                UpdatePendingChannel(eventLoop, channel);
                break;
        }
        channelElement = channelElement->next;
    }
    eventLoop->pendingHead = eventLoop->pendingTail = nullptr;
    eventLoop->isHandlePending = 0;
    pthread_mutex_unlock(&eventLoop->mutex);
    return 0;
}

int AddPendingChannel(EventLoop* eventLoop, Channel* channel)
{
    int sockfd = channel->sockfd;
    ChannelMap* cmap = eventLoop->channelMap;
    if (sockfd < 0) return 0;
    if (sockfd > cmap->entryNum) {
        if (makeSpace(cmap, sockfd, sizeof(Channel*)) == -1)
            return -1;
    }

    if (cmap->entries[sockfd] == nullptr) {
        cmap->entries[sockfd] = channel;
        EventDispatcher* dispatcher = eventLoop->eventDispatcher;
        dispatcher->add(eventLoop, channel);
        return 1;
    }
    return 0;
}

int DelPendingChannel(EventLoop* eventLoop, Channel* channel)
{
    int sockfd = channel->sockfd;
    ChannelMap* cmap = eventLoop->channelMap;
    if (sockfd < 0)
        return 0;
    if (sockfd >= cmap->entryNum)
        return -1;
    auto* delchannel = static_cast<Channel *>(cmap->entries[sockfd]);
    EventDispatcher* dispatcher = eventLoop->eventDispatcher;
    int retval = 0;
    if (dispatcher->del(eventLoop, delchannel) == -1)
        retval = -1;
    else
        retval = 1;
    cmap->entries[sockfd] = nullptr;
    return retval;
}

int UpdatePendingChannel(EventLoop* eventLoop, Channel* channel)
{
    ChannelMap* cmap = eventLoop->channelMap;
    int sockfd = channel->sockfd;
    if (sockfd < 0 || cmap->entries[sockfd] == nullptr)
        return -1;
    EventDispatcher* dispatcher = eventLoop->eventDispatcher;
    dispatcher->update(eventLoop, channel);
}


int NotifyChannel(void* data)
{
    auto* eventLoop = static_cast<EventLoop*>(data);
    char one;
    ssize_t n = read(eventLoop->socketPair[1], &one, sizeof one);
    if (n != sizeof(one))
        printf("wake up failed");
    else
        printf("wakeup %s", eventLoop->threadName);
}

EventLoop* EventLoopInit (char* threadName)
{
    auto* eventLoop = new EventLoop();
    pthread_mutex_init(&eventLoop->mutex, nullptr);
    pthread_cond_init(&eventLoop->cond, nullptr);

    if (threadName != nullptr)
        eventLoop->threadName = threadName;
    else
        eventLoop->threadName = "Main Thread";

    eventLoop->quit = 0;
    eventLoop->channelMap = new ChannelMap();
    initChannelMap(eventLoop->channelMap);

#ifdef EPOLL_ENABLE
    eventLoop->eventDispatcher = &epollDispatcher;
#else
    eventLoop->eventDispatcher = &PollDispatcher;
#endif
    eventLoop->eventDispatcherData = eventLoop->eventDispatcher->Init(eventLoop);

    eventLoop->ownerThreadId = pthread_self();
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair) < 0) {
        printf("socketpair set failed");
    }
    eventLoop->isHandlePending = 0;
    eventLoop->pendingHead = eventLoop->pendingTail = nullptr;

    Channel* channel = NewChannel(eventLoop->socketPair[1], EventType::READ, NotifyChannel, nullptr, eventLoop);
    // register sub reactor thread as channel to main reactor thread
    PushChannel(eventLoop, channel);
    return eventLoop;
}

int EventLoopRun (EventLoop* eventLoop)
{
    assert(eventLoop != nullptr);
    EventDispatcher* dispatcher = eventLoop->eventDispatcher;
    if (eventLoop->ownerThreadId != pthread_self())
        exit(1);

    struct timeval tval{};
    tval.tv_sec = 1;
    while (!eventLoop->quit) {
        // block here to wait I/O event occurs, like epoll_wait.
        dispatcher->Dispatch(eventLoop, &tval);
        // event occurs, handle the pending channels
        HandlePendingChannels(eventLoop);
    }
    return 0;
}

void WakeupSubEventLoop (EventLoop* eventLoop)
{
    char one = 'a';
    ssize_t n = write(eventLoop->socketPair[0], &one, sizeof one);
    if (n != sizeof(one))
        printf("wakeup event loop thread failed");
}

int ActivateChannel (EventLoop* eventLoop, int fd, int revents)
{
    ChannelMap* cmap = eventLoop->channelMap;
    if (fd < 0) return 0;
    if (fd >= cmap->entryNum) return -1;

    auto *channel = static_cast<Channel *>(cmap->entries[fd]);
    assert(fd == channel->sockfd);

    if (revents & EventType::READ) {
        if (channel->eventReadCallback)
            channel->eventReadCallback(channel->handle);
    }
    if (revents & EventType::WRITE) {
        if (channel->eventWriteCallback)
            channel->eventWriteCallback(channel->handle);
    }
}


int RegisterChannel(EventLoop* eventLoop, Channel* channel, int type)
{
    pthread_mutex_lock(&eventLoop->mutex);
    assert(eventLoop->isHandlePending == 0);

    auto* channelElement = new ChannelElement;
    channelElement->channel = channel;
    channelElement->operationType = type;
    channelElement->next = nullptr;
    if (eventLoop->pendingHead == nullptr)
        eventLoop->pendingHead = eventLoop->pendingTail = channelElement;
    else {
        eventLoop->pendingTail->next = channelElement;
        eventLoop->pendingTail = eventLoop->pendingTail->next;
    }
    pthread_mutex_unlock(&eventLoop->mutex);

    if (eventLoop->ownerThreadId == pthread_self())
        HandlePendingChannels(eventLoop);
    else
        WakeupSubEventLoop(eventLoop);
    return 0;
}

int PushChannel (EventLoop* eventLoop, Channel* channel)
{
    return RegisterChannel(eventLoop, channel, 1);
}

int PopChannel (EventLoop* eventLoop, Channel* channel)
{
    return RegisterChannel(eventLoop, channel, 2);
}

int ModChannel (EventLoop* eventLoop, Channel* channel)
{
    return RegisterChannel(eventLoop, channel, 3);
}