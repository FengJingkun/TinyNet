## TinyNet

TinyNet是一个采用reactor模式，基于事件分发和回调的并发网络编程框架。

<br>

## Reactor Model

### EventLoop

一个处于无限循环的事件分发器，一旦有事件发生，它会根据事件类型调用相应的回调函数，完成事件的处理。

具体实现中，EventLoop使用poll或epoll将一个线程阻塞，等待具体的IO事件发生。

<br>

### Channel

Channel代表注册到EventLoop上的事件对象，如注册到EventLoop上的socket监听事件。

Channel包含套接字，事件类型，事件处理函数等信息。

<br>

### Acceptor

Acceptor表示服务端socket的监听器，它同样会作为一个Channel注册到EventLoop上，以便监听socket，从已连接队列中获取socket连接。

<br>

### EventDispatcher

事件分发器的抽象，该结构体包含一系列函数指针，具体实现由poll或epoll来完成。

<br>

### ChannelMap

ChannelMap保存了socket到Channel的映射。

当事件发生时，EventLoop就可根据事件对应的socket来快速找到对应的Channel对象，进而找到对应的事件处理函数。

ChannelMap中包含一个Entries数组，数组下标为socket fd，数组元素为对应的Channel地址。当给定fd大于数组长度时，成倍扩充数组，直至数组长度能覆盖fd。
