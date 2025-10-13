#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common/typedef.h"

typedef struct {
    ConnFD fd;
    sockaddr_in address;
    std::queue<std::string> msg_queue;
} ClientInfo;

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
    std::vector<std::pair<ConnFD, std::string>> getClientsList() const;

private:
    bool is_running_;
    SocketFD sfd_;
    SocketAddrIn address_;
    std::unordered_map<ConnFD, ClientInfo> clients_; 
};