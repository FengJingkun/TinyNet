//
// Created by ErMo on 2021/2/8.
//

#ifndef TINYNET_TCPCONNECTION_H
#define TINYNET_TCPCONNECTION_H

#include "EventLoop.h"
#include "Channel.h"

class TCPConnection;
using ConnectionCompleteCallback = int (*) ();
using MessageCallback = int (*) ();
using WriteCompleteCallback = int (*) ();
using ConnectionCloseCallback = int (*) ();

class TCPConnection {
public:
    EventLoop* eventLoop;
    Channel* channel;
    char* name;

    Buffer* inputBuffer;
    Buffer* outputBuffer;

    ConnectionCompleteCallback establishedCallback;
    MessageCallback messageCallback;
    WriteCompleteCallback writeCompleteCallback;
    ConnectionCloseCallback closeCallback;

    void* data;
    void* request;
    void * response;

    void Initialize(int, EventLoop*, ConnectionCompleteCallback, MessageCallback,
                    WriteCompleteCallback, ConnectionCloseCallback);

    int SendData(void*, int);

    int SendBuffer(Buffer*);

    void Shutdown();
};


#endif //TINYNET_TCPCONNECTION_H
