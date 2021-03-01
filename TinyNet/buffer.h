//
// Created by ErMo on 2021/2/28.
//

#ifndef REACTORSERVER_BUFFER_H
#define REACTORSERVER_BUFFER_H

#include "common.h"
#include "log.h"


class buffer {
public:
    buffer(): readIndex(0), writeIndex(0), totalSize(1024), data(nullptr), CRLF("\r\n")
    {
        data = static_cast<char*>(std::malloc(1024));
    }

    char* locate_CRLF() const
    {
        auto* crlf = static_cast<char *>(memmem(this->data + this->readIndex,
                                                this->readable_size(),
                                                this->CRLF, 2));
        return crlf;
    }

    /** get request data from buffer to process */
    char read_buffer()
    {
        char byte = this->data[this->readIndex++];
        return byte;
    }

    /** write response data to buffer after processing */
    void write_buffer(void* rawData, int nbytes)
    {
        if (data != nullptr) {
            this->make_room(nbytes);
            std::memcpy(this->data + this->writeIndex, rawData, nbytes);
            this->writeIndex += nbytes;
        }
    }

    /** receive request data from socket and write it to buffer */
    int recv_from_socket(int fd)
    {
        int writableBytes = this->writeable_size();
        char additional_buffer[1024];

        struct iovec vec[2];
        vec[0].iov_base = this->data + this->writeIndex;
        vec[0].iov_len = writableBytes;
        vec[1].iov_base = additional_buffer;
        vec[1].iov_len = sizeof additional_buffer;

        int nbytes = readv(fd, vec, 2);
        if (nbytes < 0) {
            return -1;
        }
        else if (nbytes <= writableBytes) {
            this->writeIndex += nbytes;
        }
        else { // result > max_writable
            // 可能引起移动，也可能realloc扩容
            this->make_room(nbytes);
            this->writeIndex = this->totalSize;
            this->write_buffer(additional_buffer, nbytes - writableBytes);
        }

        return nbytes;
    }

    int readable_size() const
    {
        return this->writeIndex - this->readIndex;
    }

    int writeable_size() const
    {
        return this->totalSize - this->writeIndex;
    }

    int front_space_size() const
    {
        return this->readIndex;
    }

    void buffer_free()
    {
        std::free(this->data);
        this->data = nullptr;
        this->readIndex = 0;
        this->writeIndex = 0;
        this->totalSize = 0;
    }

protected:
    void make_room(int size)
    {
        if (this->writeable_size() > size)
            return;
        else if (this->writeable_size() + this->front_space_size() > size) {
            int readable = this->readable_size();
            for (int i = 0; i < readable; ++i) {
                std::memcpy(this->data + i, this->data + this->readIndex + i, 1);
            }
            this->readIndex = 0;
            this->writeIndex = readable;
        }
        else { // expand the buffer
            void* tmp = std::realloc(this->data, this->totalSize + size);
            this->data = static_cast<char*>(tmp);
            this->totalSize += size;
        }
    }

public:
    int readIndex;  // buffer read position
    int writeIndex; // buffer write position
    int totalSize;  // buffer size
    char* data;     // raw data in buffer
    const char* CRLF;
};

#endif //REACTORSERVER_BUFFER_H