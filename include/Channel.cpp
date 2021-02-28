//
// Created by ErMo on 2021/2/3.
//

#include "Channel.h"

Channel* NewChannel (int fd, int event, Event_Read_Callback RdCallbk, Event_Write_Callback WrCallbk, void* handle)
{
    Channel* channel = new Channel();
    channel->sockfd = fd;
    channel->event = event;
    channel->eventReadCallback = RdCallbk;
    channel->eventWriteCallback = WrCallbk;
    channel->handle = handle;
    return channel;
}

int isChannelWritable (Channel* channel)
{
    return channel->event & EventType::Writable;
}

int enalbeChannelWrite (Channel* channel)
{
    EventLoop* eventLoop = static_cast<EventLoop*>(channel->handle);
    channel->event |= EventType::Writable;
    updateChannelEvent(eventLoop, channel);
}

int disableChannelWrite (Channel* channel)
{
    EventLoop* eventLoop = static_cast<EventLoop*>(channel->handle);
    channel->event &= ~EventType::Writable;
    updateChannelEvent(eventLoop, channel);
}