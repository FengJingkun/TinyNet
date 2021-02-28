//
// Created by ErMo on 2021/2/28.
//

#include "event_loop.h"


event_loop* event_loop::event_loop_init(std::string &&name)
{
    std::string realName = name.empty()? "mainEventLoop": std::move(name);

    event_dispatcher* pDispatcher = new poll_dispatcher("poll");

    auto* pEventLoop = new event_loop(std::move(realName), pDispatcher);

    auto dispatcher_callback = std::bind(&event_loop::channel_event_activate,
                                         pEventLoop,
                                         std::placeholders::_1,
                                         std::placeholders::_2);
    pDispatcher->set_callback(dispatcher_callback);

    channel* pChannel = channel::channel_new(pEventLoop->socketPair[1],
                                             EventType::EventRead);

    auto channel_read_callback = std::bind(&event_loop::handle_wakeup, pEventLoop);
    pChannel->set_read_callback(channel_read_callback);

    pEventLoop->add_channel_event(pChannel);

    return pEventLoop;
}

int event_loop::event_loop_run()
{
    if (this->threadId != pthread_self()) {
        LOG_ERROR("%s: Not running on bind Thread-%d, Current tid = %d\n",
                  this->get_loopName(),
                  this->get_threadId(),
                  pthread_self());
        exit(1);
    }

    LOG_MSG("EventLoop running: %s, thread = %d\n",
            this->get_loopName(),
            this->threadId);

    while (!this->isQuit) {
        /** block here to wait I/O event, then active channels **/
        this->dispatcher->dispatch();

        /** I/O event arrives, start handle the pending channels **/
        this->handle_pending_channel();
    }

    LOG_MSG("EventLoop quit: %s\n", this->loopName.c_str());
    return 0;
}

int event_loop::channel_event_activate(void* data, int fd, int revents)
{
    auto pEventLoop = static_cast<event_loop*>(data);
    if (pEventLoop->channelMap.find(fd) == pEventLoop->channelMap.end()) {
        LOG_ERROR("%s: Fd = %d is NOT Found\n", pEventLoop->get_loopName(), fd);
        return -1;
    }

    channel* pChannel = pEventLoop->channelMap[fd];
    assert(pChannel->get_channel_fd() == fd);

    if (revents & EventType::EventRead) {
        pChannel->channel_read();
    }
    if (revents & EventType::EventWrite) {
        pChannel->channel_write();
    }

    return 0;
}

void event_loop::event_loop_wakeup()
{
    char one = 'a';
    ssize_t n = write(this->socketPair[0], &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("Wake up SubEventLoop %s failed\n", this->get_loopName());
        exit(1);
    }
    LOG_MSG("%s: Notify to wake up\n", this->get_loopName());
}

void event_loop::handle_wakeup(void* data)
{
    auto pEventLoop = reinterpret_cast<event_loop*>(data);

    char one;
    ssize_t n = read(pEventLoop->get_SocketPair(), &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("%s: SocketPair Read Failed\n", pEventLoop->get_loopName());
        exit(1);
    }

    LOG_MSG("%s: Wake up\n", pEventLoop->get_loopName());
}

int event_loop::add_channel_event(channel* pChannel)
{
    LOG_MSG("Add Channel to EventLoop %s\n", this->get_loopName());
    return this->do_channel_event(pChannel, ChannelOpt::ADD);
}

int event_loop::remove_channel_event(channel* pChannel)
{
    LOG_MSG("Remove Channel to EventLoop %s\n", this->get_loopName());
    return this->do_channel_event(pChannel, ChannelOpt::DEL);
}

int event_loop::update_channel_event(channel* pChannel)
{
    LOG_MSG("Update Channel to EventLoop %s\n", this->get_loopName());
    return this->do_channel_event(pChannel, ChannelOpt::MOD);
}

int event_loop::do_channel_event(channel* pChannel, int operation)
{
    pthread_mutex_lock(&this->mutex);

    auto* pChannelElement = new channel_element();
    pChannelElement->operation = operation;
    pChannelElement->element = pChannel;

    this->pendingChannelList.push(pChannelElement);

    pthread_mutex_unlock(&this->mutex);

    if (this->threadId == pthread_self()) {
        this->handle_pending_channel();
    }
    else {
        this->event_loop_wakeup();
    }
    return 0;
}

int event_loop::handle_pending_channel()
{
    pthread_mutex_lock(&this->mutex);

    while (!this->pendingChannelList.empty()) {

        auto pChannelElement = this->pendingChannelList.front();
        this->pendingChannelList.pop();

        int operation = pChannelElement->operation;
        channel* pChannel = pChannelElement->element;

        if (operation == ChannelOpt::ADD) {
            this->handle_pending_add(pChannel);
        }
        else if (operation == ChannelOpt::DEL) {
            this->handle_pending_remove(pChannel);
        }
        else if (operation == ChannelOpt::MOD) {
            this->handle_pending_update(pChannel);
        }

        channel_element::destroy(pChannelElement);
        pChannelElement = nullptr;
    }

    pthread_mutex_unlock(&this->mutex);

    return 0;
}

int event_loop::handle_pending_add(channel* pChannel)
{
    int eventFd = pChannel->get_channel_fd();
    int eventType = pChannel->get_channel_type();

    LOG_MSG("%s: Register Fd = %d to dispatcher, EventType = %d\n",
            this->get_loopName(), eventFd, eventType);

    int res = 0;
    if (this->channelMap.find(eventFd) == this->channelMap.end()) {
        this->channelMap[eventFd] = pChannel;
        res = this->dispatcher->add(eventFd, eventType);
    }

    if (res < 0) {
        LOG_ERROR("%s register Channel failed, Fd = %d\n",
                  this->get_loopName(), eventFd);
        return -1;
    }

    return 0;
}

int event_loop::handle_pending_remove(channel* pChannel)
{
    int eventFd = pChannel->get_channel_fd();
    int eventType = pChannel->get_channel_type();

    LOG_MSG("%s: Remove Fd = %d in dispatcher, EventType = %d\n",
            this->get_loopName(), eventFd, eventType);

    int res = 0;
    if (this->channelMap.find(eventFd) != this->channelMap.end()) {
        res = this->dispatcher->del(eventFd, eventType);

        channel::channel_free(this->channelMap[eventFd]);
        this->channelMap[eventFd] = nullptr;
        this->channelMap.erase(eventFd);
    }
    else {
        LOG_ERROR("%s: Channel NOT Found, Fd = %d\n",
                  this->get_loopName(), eventFd);
        return -1;
    }

    if (res < 0) {
        LOG_ERROR("%s delete Channel failed, Fd = %d\n",
                  this->get_loopName(), eventFd);
        return -1;
    }
    return 0;
}

int event_loop::handle_pending_update(channel* pChannel)
{
    int eventFd = pChannel->get_channel_fd();
    int eventType = pChannel->get_channel_type();

    LOG_MSG("%s: Update Fd = %d in dispatcher, EventType = %d\n",
            this->get_loopName(), eventFd, eventType);

    int res = 0;
    if (this->channelMap.find(eventFd) != this->channelMap.end()) {
        this->channelMap[eventFd] = pChannel;
        res = this->dispatcher->update(eventFd, eventType);
    }
    else {
        LOG_ERROR("%s: Channel NOT Found, Fd = %d\n",
                  this->get_loopName(), eventFd);
        return -1;
    }

    if (res < 0) {
        LOG_ERROR("%s update Channel failed, Fd = %d\n",
                  this->get_loopName(), eventFd);
        return -1;
    }

    return 0;
}