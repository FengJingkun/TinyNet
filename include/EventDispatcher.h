//
// Created by ErMo on 2021/2/3.
//

#ifndef TINYNET_EVENTDISPATCHER_H
#define TINYNET_EVENTDISPATCHER_H

class EventLoop;
class Channel;

class EventDispatcher {
public:
    EventDispatcher() { name = nullptr; }

    const char* name;

    virtual void* Init(EventLoop*) = 0;

    virtual int Add(EventLoop*, Channel*) = 0;

    virtual int Del(EventLoop*, Channel*) = 0;

    virtual int Update(EventLoop*, Channel*) = 0;

    virtual int Dispatch(EventLoop*, struct timeval*) = 0;

    virtual void Clear(EventLoop*) = 0;
};

#endif //TINYNET_EVENTDISPATCHER_H
