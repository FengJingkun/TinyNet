//
// Created by ErMo on 2021/2/28.
//

#include "callbacks.h"


char rot13_char(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}


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
        char serverChar = rot13_char(clientChar);
        sendBuffer->write_buffer(&serverChar, sizeof serverChar);
    }
    return 0;
}


void onWriteCompleted(tcp_connection* pTCPConnection)
{
    printf("write completed\n");
}


void onConnectionClosed(tcp_connection* pTCPConnection)
{
    printf("connection closed\n");
}