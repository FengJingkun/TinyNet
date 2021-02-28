//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_TCP_CONNECTION_H
#define REACTORSERVER_TCP_CONNECTION_H

#include "common.h"
#include "event_loop.h"
#include "channel.h"
#include "buffer.h"


class tcp_connection {
public:
    tcp_connection(int connFd, event_loop* pLoop)
            : pEventLoop(pLoop)
            , pChannel(nullptr)
            , pRecvBuffer(nullptr)
            , pSendBuffer(nullptr)
            , connectionAcceptedCallback(nullptr)
            , messageCallback(nullptr)
            , writeCompletedCallback(nullptr)
            , connectionClosedCallback(nullptr)
            , data(nullptr)
            , request(nullptr)
            , response(nullptr)
    {
        std::string name = "connection-" + std::to_string(connFd);
        this->connName = std::move(name);

        this->pRecvBuffer = new buffer();
        this->pSendBuffer = new buffer();

        this->init(connFd);
    }

    void set_callbacks(connection_accepted_callback acceptedCbk,
                       message_process_callback messageCbk,
                       write_completed_callback writeCompletedCbk,
                       connection_closed_callback connectionClosedCbk)
    {
        this->connectionAcceptedCallback = std::move(acceptedCbk);
        this->messageCallback = std::move(messageCbk);
        this->writeCompletedCallback = std::move(writeCompletedCbk);
        this->connectionClosedCallback = std::move(connectionClosedCbk);
    }

    buffer* get_sendBuffer()
    {
        return this->pSendBuffer;
    }

    buffer* get_recvBuffer()
    {
        return this->pRecvBuffer;
    }

    void destroy()
    {
        this->pRecvBuffer->buffer_free();
        this->pSendBuffer->buffer_free();
        this->pRecvBuffer = nullptr;
        this->pSendBuffer = nullptr;

        this->pEventLoop = nullptr;
        this->pChannel = nullptr;

        this->connectionClosedCallback = nullptr;
        this->messageCallback = nullptr;
        this->writeCompletedCallback = nullptr;
        this->connectionClosedCallback = nullptr;

        this->data = nullptr;
        this->request = nullptr;
        this->response = nullptr;
    }

    /** close the connection, and free the related resource */
    void handle_connection_closed();
    /** Receive request data from socket, then store it to recvBuffer */
    static void handle_connection_read(tcp_connection*);
    /** Send response data stored in sendBuffer to socket */
    static void handle_connection_write(tcp_connection*);

    static int connection_send_buffer(tcp_connection*);

    tcp_connection(const tcp_connection&) = delete;
    tcp_connection& operator=(const tcp_connection&) = delete;
    ~tcp_connection() = default;

protected:
    void init(int);

private:
    std::string connName;

    event_loop* pEventLoop;
    channel* pChannel;

    /** RecvBuffer receive request data from socket first,
      * Then Framework process data through RecvBuffer */
    buffer* pRecvBuffer;
    /** SendBuffer send response data to socket,
      * Framework write response data to SendBuffer */
    buffer* pSendBuffer;

    connection_accepted_callback connectionAcceptedCallback;
    message_process_callback messageCallback;
    write_completed_callback writeCompletedCallback;
    connection_closed_callback connectionClosedCallback;

    void* data;
    void* request;
    void* response;
};

#endif //REACTORSERVER_TCP_CONNECTION_H