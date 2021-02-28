//
// Created by ErMo on 2021/2/8.
//

#include "TCPConnection.h"

void TCPConnection::Initialize(int fd, EventLoop* pEventLoop,
                               ConnectionCompleteCallback connDoneCallback,
                               MessageCallback msgCallback,
                               WriteCompleteCallback wrDoneCallback,
                               ConnectionCloseCallback quitCallback)
{
    this->establishedCallback = connDoneCallback;
    this->messageCallback = msgCallback;
    this->writeCompleteCallback = wrDoneCallback;
    this->closeCallback = quitCallback;
    this->eventLoop = pEventLoop;
    this->inputBuffer = newBuffer();
    this->outputBuffer = newBuffer();

    char* buf = new char[16];
    sprintf(buf, "Connection-%d", fd);
    name = buf;

    Channel* channel1 = NewChannel(fd, EventType::READ, handleRead, handleWrite, this);
    this->channel = channel1;

    if (this->establishedCallback)
        this->establishedCallback();

    PushChannel(this->eventLoop, channel1);
}

int TCPConnection::SendData(void* datas, int size)
{

}