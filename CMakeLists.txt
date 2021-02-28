cmake_minimum_required(VERSION 3.17)
project(ReactorServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(ReactorServer main.cpp TinyNet/common.h TinyNet/log.h TinyNet/acceptor.h TinyNet/channel.h TinyNet/event_dispatcher.h TinyNet/poll_dispatcher.cpp TinyNet/poll_dispatcher.h TinyNet/event_loop.cpp TinyNet/event_loop.h TinyNet/log.cpp TinyNet/thread_pool.cpp TinyNet/thread_pool.h TinyNet/event_loop_thread.cpp TinyNet/event_loop_thread.h TinyNet/buffer.h TinyNet/tcp_server.cpp TinyNet/tcp_server.h TinyNet/callbacks.cpp TinyNet/callbacks.h TinyNet/tcp_connection.cpp TinyNet/tcp_connection.h)