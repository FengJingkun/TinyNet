//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_TCP_CONNECTION_H
#define REACTORSERVER_TCP_CONNECTION_H

#include <utility>

#include "common.h"
#include "event_loop.h"
#include "channel.h"
#include "buffer.h"


class tcp_connection {
public:
    tcp_connection()
            : pEventLoop(nullptr)
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
        this->pRecvBuffer = new buffer();
        this->pSendBuffer = new buffer();
    }

    void init(int, event_loop*,
              connection_accepted_callback,
              message_process_callback,
              write_completed_callback,
              connection_closed_callback);

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

        this->connectionAcceptedCallback = nullptr;
        this->messageCallback = nullptr;
        this->writeCompletedCallback = nullptr;
        this->connectionClosedCallback = nullptr;

        this->data = nullptr;
        this->request = nullptr;
        this->response = nullptr;
    }

    /** Close the connection, and free the related resource */
    void handle_connection_closed();
    /** Register as Channel read callback */
    static void handle_connection_read(tcp_connection*);
    /** Register as Channel write callback */
    static void handle_connection_write(tcp_connection*);

    /**  */
//    int connection_send_response();

    tcp_connection(const tcp_connection&) = delete;
    tcp_connection& operator=(const tcp_connection&) = delete;
    ~tcp_connection() = default;

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

    /** call after the new connection accepted */
    connection_accepted_callback connectionAcceptedCallback;
    /** call to process the request data */
    message_process_callback messageCallback;
    /** call after response data has been sent */
    write_completed_callback writeCompletedCallback;
    /** the tcp_connection obj will destroy after calling this func */
    connection_closed_callback connectionClosedCallback;

    void* data;
    void* request;
    void* response;
};

#endif //REACTORSERVER_TCP_CONNECTION_H