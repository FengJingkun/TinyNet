//
// Created by ErMo on 2021/2/7.
//

#ifndef TINYNET_ACCEPTOR_H
#define TINYNET_ACCEPTOR_H

#include "common.h"

class Acceptor {
public:
    Acceptor() = default;
    void Initialize(int);

public:
    int mListenPort;
    int mListenFd;
};


#endif //TINYNET_ACCEPTOR_H
