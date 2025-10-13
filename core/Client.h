#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common/typedef.h"

class Client {
public:
    Client();
    virtual ~Client();

    Client(const Client&) = default;
    Client& operator=(const Client&) = default;

    void start();
    void stop();
    void setRunning(bool running);
    bool isRunning() const;
    SocketFD connectToServer(std::string addr, Port port);
    int32_t sendToServer(SocketFD sfd, const std::string& message);

private:
    bool is_running_;
    std::vector<SocketFD> sfd_list_;
};