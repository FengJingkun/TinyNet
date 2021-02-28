//
// Created by ErMo on 2021/2/4.
//

#include "ChannelMap.h"

int makeSpace(ChannelMap* cmap, size_t slot, int msize)
{
    if (cmap->entryNum <= slot)
    {
        size_t entryNum = (cmap->entryNum != 0) ? cmap->entryNum : 32;
        while (entryNum <= slot)
            entryNum <<= 1;

        void** tmp = nullptr;
        tmp = static_cast<void**>(realloc(cmap->entries, entryNum * msize));
        if (tmp == nullptr)
            return -1;

        memset(&tmp[cmap->entryNum], 0, (entryNum - cmap->entryNum) * msize);
        cmap->entryNum = entryNum;
        cmap->entries = tmp;
    }
    return 0;
}

void initChannelMap(ChannelMap* cmap)
{
    cmap->entries = nullptr;
    cmap->entryNum = 0;
}

void clearChannelMap(ChannelMap* cmap)
{
    if (cmap->entries != nullptr)
    {
        for (size_t i = 0; i < cmap->entryNum; ++i) {
            if (cmap->entries[i] != nullptr)
            {
                free(cmap->entries[i]);
                cmap->entries[i] = nullptr;
            }
        }
        free(cmap->entries);
        cmap->entries = nullptr;
    }
    cmap->entryNum = 0;
}