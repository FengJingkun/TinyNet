//
// Created by ErMo on 2021/2/3.
//

#ifndef TINYNET_CHANNEL_H
#define TINYNET_CHANNEL_H

#include "common.h"
#include "EventLoop.h"

using Event_Read_Callback = int (*) (void*);
using Event_Write_Callback = int (*) (void*);

struct Channel {
    int sockfd;
    int event;

    Event_Read_Callback eventReadCallback;
    Event_Write_Callback eventWriteCallback;
    void* handle;
};

struct ChannelElement {
    int operationType;
    Channel* channel;
    ChannelElement* next;
};

Channel* NewChannel (int, int, Event_Read_Callback, Event_Write_Callback, void*);

int isChannelWritable (Channel*);

int enalbeChannelWrite (Channel*);

int disableChannelWrite (Channel*);

#endif //TINYNET_CHANNEL_H