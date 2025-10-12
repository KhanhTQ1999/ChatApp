#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common/typedef.h"

class Server {
public:
    Server(IPAddress addr = INADDR_ANY, Port port = 5000);
    virtual ~Server();

    Server(const Server&) = default;
    Server& operator=(const Server&) = default;

    void start();
    void stop();
    void listenClient(int32_t client_fd);
    void sendToClient(int32_t client_fd, const std::string& message);

    void setRunning(bool running);
    bool isRunning() const;
    void setPort(Port port);
    Port getPort() const;
    void setAddress(IPAddress addr);
    IPAddress getAddress() const;

private:
    SocketFD sfd_;
    bool is_running_;
    fd_set readfds_;
    SocketAddrIn address_;
    std::vector<int32_t> clients_; 
};