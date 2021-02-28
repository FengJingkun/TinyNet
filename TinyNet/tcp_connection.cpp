//
// Created by ErMo on 2021/2/28.
//

#include "tcp_connection.h"

void tcp_connection::init(int connFd, event_loop* pSubLoop,
                          connection_accepted_callback acceptedCbk,
                          message_process_callback messageCbk,
                          write_completed_callback writeCompletedCbk,
                          connection_closed_callback connectionClosedCbk)
{
    std::string name = "connection-" + std::to_string(connFd);
    this->connName = std::move(name);

    this->pEventLoop = pSubLoop;

    this->connectionAcceptedCallback = std::move(acceptedCbk);
    this->messageCallback = std::move(messageCbk);
    this->writeCompletedCallback = std::move(writeCompletedCbk);
    this->connectionClosedCallback = std::move(connectionClosedCbk);

    channel* connChannel = channel::channel_new(connFd, EventType::EventRead);

    auto read_callback = std::bind(&tcp_connection::handle_connection_read, this);
    auto write_callback = std::bind(&tcp_connection::handle_connection_write, this);
    connChannel->set_read_callback(std::move(read_callback));
    connChannel->set_write_callback(std::move(write_callback));

    this->pEventLoop->add_channel_event(connChannel);

    this->pChannel = connChannel;
    if (this->connectionAcceptedCallback != nullptr)
        this->connectionAcceptedCallback();
}

void tcp_connection::handle_connection_closed()
{
    /** this func will destroy the channel */
    this->pEventLoop->remove_channel_event(this->pChannel);
    if (this->connectionClosedCallback != nullptr)
        this->connectionClosedCallback();
    this->destroy();
}

void tcp_connection::handle_connection_read(tcp_connection* pTCPConnection)
{
    buffer* pRecvBuffer = pTCPConnection->pRecvBuffer;
    channel* pChannel = pTCPConnection->pChannel;

    int fd = pChannel->get_channel_fd();

    /** RecvBuffer receive request data from socket first */
    int n = pRecvBuffer->buffer_socket_read(fd);
    if (n > 0) {
        /** Store request data to recvBuffer successfully, then processing request */
        if (pTCPConnection->messageCallback != nullptr)
            pTCPConnection->messageCallback();
    }
    else {
        LOG_ERROR("%s: Receive request data from socket failed, close the connection",
                  pTCPConnection->connName.c_str());
        pTCPConnection->handle_connection_closed();
        pTCPConnection = nullptr;
    }
}


void tcp_connection::handle_connection_write(tcp_connection* pTCPConnection)
{
    event_loop* pEventLoop = pTCPConnection->pEventLoop;
    /** Ensure current connection was processed by its owner SubEventLoop */
    assert(pEventLoop->get_threadId() == pthread_self());

    buffer* pSendBuffer = pTCPConnection->pSendBuffer;
    channel* pChannel = pTCPConnection->pChannel;

    ssize_t nwrited = write(pChannel->get_channel_fd(),
                            pSendBuffer->data + pSendBuffer->readIndex,
                            pSendBuffer->readable_size());
    if (nwrited > 0) {
        if (pSendBuffer->readable_size() == 0) {
            /** All response data has been sent */
            pChannel->channel_write_disable();
            pEventLoop->update_channel_event(pChannel);
        }
        if (pTCPConnection->writeCompletedCallback != nullptr)
            pTCPConnection->writeCompletedCallback();
    }
    else {
        LOG_ERROR("%s: Send response data to socket failed, close the connection",
                  pTCPConnection->connName.c_str());
        pTCPConnection->handle_connection_closed();
        pTCPConnection = nullptr;
    }
}

/**
 * 应用程序将处理完的数据写入buffer, 由框架负责将buffer中的数据通过socket发送出去
 * 先往socket发送数据，若发送成功则不使用buffer，否则将剩余未发送数据写入buffer
 **/
int tcp_connection::connection_send_buffer(tcp_connection* pTCPConnection)
{
    channel* pChannel = pTCPConnection->pChannel;
    buffer* pSendBuffer = pTCPConnection->pSendBuffer;
    int toSendSize = pSendBuffer->readable_size();

    pSendBuffer->readIndex += toSendSize;
    return 0;
}