## TinyNet

TinyNet是一个C++11编写的，采用主从reactor模式，基于事件分发和回调的多线程服务端框架。

<br>

<br>

## Model

并发模型为主从Reactor + 非阻塞IO + 线程池，新连接使用Round Robin分配。

并发模型使用主从Reactor模式，引入多线程提高并发度。特别地，为了避免线程频繁创建和销毁带来的开销，使用线程池，在服务启动时创建固定数量的线程，同时使用poll / epoll作为IO多路复用的实现方式。

<br>

<br>

## Technical Points

- 主从Reactor模式，使用poll / epoll的IO多路复用，非阻塞IO

- epoll使用Edge Trigger模式

- 使用线程池，避免线程频繁创建和销毁带来的开销

- 主线程负责accept连接，并通过Round Robin将事件分发给子线程，子线程负责该事件的读写

- 使用socketpair实现子线程的唤醒

- 使用缓冲区，负责从socket读取网络数据，暂存服务端处理后的数据，并支持动态扩容

<br>

<br>

## Key  Class Description

### event_loop

一个处于无限循环的事件分发器，一旦有事件发生，它会根据事件类型调用相应的回调函数，完成事件的处理。

每个线程单独绑定一个event_loop。

<br>

### event_dispatcher

事件分发机制的抽象类，具体由poll或者epoll实现，event_loop通过指针管理dispatcher对象。

<br>

### channel

channel是对各种注册到event_loop上的事件的抽象，如注册到event_loop上的监听事件，socket读写事件等。

每个channel对象对应一个唯一的event_loop，由event_loop的pendingChannelList进行管理。

<br>

### acceptor

acceptor是服务端监听器，它会作为一个channel对象绑定到主线程的event_loop上，通过注册的回调函数进行tcp连接完成的事件检测和分发。

<br>

### thread_pool

单例类，thread_pool维护了一个sub-reactor的线程列表，每当有新连接建立时，主线程从thread_pool中通过Round Robin获取子线程的event_loop，并将新连接对应的channel注册到sub-event_loop上，后续该连接的读写事件由子线程完成，从而实现了主reactor线程与IO线程的分离。

<br>

### event_loop_thread

从reactor的线程实现，子线程的event_loop由event_loop_thread管理，通过互斥锁 + 条件变量保证sub-event_loop的成功初始化。

<br>

### buffer

buffer对象屏蔽了对socket的IO操作进行了屏蔽。

读事件到达时，buffer调用socket read读取数据并保存，若空间不足则会引起扩容，保证将数据一次性读完（对应epoll的ET模式）；

服务端完成处理后将数据保存至buffer，并注册对应channel的写事件；

写事件被触发后，buffer调用socket write发送数据，若存在未发送数据，则继续注册写事件，直至数据发送完成。

<br>

### tcp_connection

tcp_connection对象对已建立的TCP连接进行描述。它的属性包含接收缓冲区，发送缓冲区，channel对象等，同时注册了一系列回调函数。其中message_process_callback是服务端处理请求数据的入口。

<br>

### tcp_server

单例类，服务端启动的主入口。绑定了event_loop，acceptor，thread_pool等对象。

<br>

<br>

## Current Progress

- 智能指针的应用：

  开发过程中对各个资源类做了较多的解耦工作，初始阶段也未明确各对象的生命周期，因此没有重点关注资源的分配和释放。为了避免智能指针的滥用，同时也为了提高效率，暂时使用了raw pointer来管理堆内存。根据RAII的原则，使用智能指针对资源类进行管理是很有必要的，因此目前正在将智能指针应用到项目中；

- HTTP报文的解析：

  应用层使用HTTP进行通信，针对HTTP报文的封装和解析是TCP服务器应该具备的功能。目前正在做的是：新增http_server类对tcp_server进行封装，该类提供parse_http_request函数来进行报文解析。

