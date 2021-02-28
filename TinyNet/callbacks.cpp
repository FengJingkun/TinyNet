//
// Created by ErMo on 2021/2/28.
//

#include "callbacks.h"


void onConnectionAccepted(tcp_connection* pTCPConnection)
{
    printf("connection completed\n");
}

int onMessage(tcp_connection* pTCPConnection)
{
    buffer* recvBuffer = pTCPConnection->get_recvBuffer();
    buffer* sendBuffer = pTCPConnection->get_sendBuffer();

    while (recvBuffer->readable_size() != 0) {
        char clientChar = recvBuffer->read_buffer();
        char serverChar = clientChar + 13;
        sendBuffer->write_buffer(&serverChar, sizeof serverChar);
    }

    tcp_connection::connection_send_buffer(pTCPConnection);
    return 0;
}

//数据通过buffer写完之后的callback
void onWriteCompleted(tcp_connection* pTCPConnection) {
    printf("write completed\n");
}

//连接关闭之后的callback
void onConnectionClosed(tcp_connection* pTCPConnection) {
    printf("connection closed\n");
}