//
// Created by ErMo on 2021/2/4.
//

#ifndef TINYNET_CHANNELMAP_H
#define TINYNET_CHANNELMAP_H

#include "Channel.h"

struct ChannelMap {
    void** entries;
    size_t entryNum;
};

int makeSpace(ChannelMap* map, int slot, int size);

void initChannelMap(ChannelMap* map);

void clearChannelMap(ChannelMap* map);

#endif //TINYNET_CHANNELMAP_H
