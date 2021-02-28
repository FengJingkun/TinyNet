//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_CALLBACKS_H
#define REACTORSERVER_CALLBACKS_H


#include "common.h"
#include "tcp_connection.h"


// typedef std::function<void(void)> connection_accepted_callback;
// typedef std::function<void(void)> message_callback;
// typedef std::function<void(void)> write_completed_callback;
// typedef std::function<void(void)> connection_closed_callback;

void onConnectionAccepted(tcp_connection *);


int onMessage(tcp_connection *);


//数据通过buffer写完之后的callback
void onWriteCompleted(tcp_connection *);


//连接关闭之后的callback
void onConnectionClosed(tcp_connection *);


#endif //REACTORSERVER_CALLBACKS_H