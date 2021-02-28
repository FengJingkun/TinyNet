//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_CALLBACKS_H
#define REACTORSERVER_CALLBACKS_H

#include "common.h"
#include "tcp_connection.h"


/** a toy request process func just for test */
char rot13_char(char);


/** call after the new connection accepted */
void onConnectionAccepted(tcp_connection *);


/** call to process the request data */
int onMessage(tcp_connection *);


/** call after response data has been sent */
void onWriteCompleted(tcp_connection *);


/** the tcp_connection obj will destroy after calling this func */
void onConnectionClosed(tcp_connection *);


#endif //REACTORSERVER_CALLBACKS_H