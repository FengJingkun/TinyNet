//
// Created by ErMo on 2021/2/7.
//

#include "PollDispatcher.h"

void* PollDispatcher::Init(EventLoop* eventLoop)
{
    name = "poll";

    auto* pollDispatcherData = new PollDispatcherData;
    pollDispatcherData->event_set = new pollfd[1024];

    int i;
    for (i = 0; i < 1024; ++i)
        pollDispatcherData->event_set[i].fd = -1;
    pollDispatcherData->event_count = 0;
    pollDispatcherData->nfds = 0;

    return pollDispatcherData;
}

int PollDispatcher::Add(EventLoop* eventLoop, Channel* channel)
{
    auto* pollDispatcherData = static_cast<PollDispatcherData*>(eventLoop->eventDispatcherData);
    int sockfd = channel->sockfd;
    int events = 0;
    if (channel->event & EventType::READ)
        events |= POLLRDNORM;
    if (channel->event & EventType::WRITE)
        events |= POLLWRNORM;

    int i = 0;
    for (; i < 1024; ++i) {
        if (pollDispatcherData->event_set[i].fd == -1) {
            pollDispatcherData->event_set[i].fd = sockfd;
            pollDispatcherData->event_set[i].events = events;
            break;
        }
    }
    if (i >= 1024)
        printf("too many clients for poll");
    return 0;
}

int PollDispatcher::Del(EventLoop* eventLoop, Channel* channel)
{
    auto* pollDispatcherData = static_cast<PollDispatcherData*>(eventLoop->eventDispatcherData);
    int sockfd = channel->sockfd;
    int i = 0;
    for (; i < 1024; ++i) {
        if (pollDispatcherData->event_set[i] == sockfd) {
            pollDispatcherData->event_set[i] = -1;
            break;
        }
    }

    if (i >= 1024)
        printf("cannot find sockfd, poll delete error");
    return 0;
}

int PollDispatcher::Update(EventLoop* eventLoop, Channel* channel)
{
    auto* pollDispatcherData = static_cast<PollDispatcherData *>(eventLoop->eventDispatcherData);
    int sockfd = channel->sockfd;

    int events = 0;
    if (channel->event & EventType::READ)
        events |= POLLRDNORM;
    if (channel->event & EventType::WRITE)
        events |= POLLWRNORM;

    int i = 0;
    for (; i < 1024; ++i) {
        if (pollDispatcherData->event_set[i].fd == sockfd) {
            pollDispatcherData->event_set[i].events = events;
            break;
        }
    }
    if (i >= 1024)
        printf("cannot find fd, poll update error");
    return 0;
}

int PollDispatcher::Dispatch(EventLoop* eventLoop, struct timeval* timeVal)
{
    auto* pollDispatcherData = static_cast<PollDispatcherData *>(eventLoop->eventDispatcherData);
    int readyNumber = 0;
    int timeWait = timeVal->tv_sec * 1000;
    readyNumber = poll(pollDispatcherData->event_set, 1024, timeWait);
    if (readyNumber < 0)
        printf("poll failed");
    else if (readyNumber == 0)
        return 0;

    int i;
    for (i = 0; i < 1024; ++i) {
        struct pollfd pollfd = pollDispatcherData->event_set[i];
        if (pollfd.fd == -1)
            continue;

        int sockfd = pollfd.fd;
        if (pollfd.revents & POLLRDNORM)
            ActivateChannel(eventLoop, sockfd, EventType::READ);
        if (pollfd.revents & POLLWRNORM)
            ActivateChannel(eventLoop, sockfd, EventType::WRITE);

        if (--readyNumber <= 0)
            break;
    }
    return 0;
}

void PollDispatcher::Clear(EventLoop* eventLoop)
{
    auto* pollDispatcherData = static_cast<PollDispatcherData *>(eventLoop->eventDispatcherData);

    delete[] pollDispatcherData->event_set;
    pollDispatcherData->event_set = nullptr;

    delete pollDispatcherData;
    eventLoop->eventDispatcherData = nullptr;
}