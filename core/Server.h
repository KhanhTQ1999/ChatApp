#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <unordered_map>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common/typedef.h"

#define BASE_PORT 5000

typedef struct {
    ConnFD fd;
    sockaddr_in address;
    std::queue<std::string> msg_queue;
} ClientInfo;

class Server {
public:
    Server(Port port = BASE_PORT);
    virtual ~Server();

    Server(const Server&) = default;
    Server& operator=(const Server&) = default;

    void start();
    void runLoop();
    void stop();
    void cleanup();
    void listenClient(int32_t client_fd);
    void sendToClient(int32_t client_fd, const std::string& message);

    void setRunning(bool running);
    bool isRunning() const;
    void setPort(Port port);
    Port getPort() const;
    void setAddress(IPAddress addr);
    IPAddress getAddress() const;
    SocketFD getSfd() const;
    std::vector<std::pair<ConnFD, std::string>> getClientsList() const;

private:
    void multiplexClients(SocketFD max_sd, fd_set& readfds, fd_set& writefds, struct timeval& timeout);
    SocketFD createSocket() const;
    SocketAddrIn buildAddress(const std::string& addr, Port port) const;
    std::pair<SocketFD, SocketAddrIn> bindAddress() const;
    struct timeval getSelectTimeout() const;
    SocketFD initializeReadFds(fd_set& readfds);
    SocketFD initializeWriteFds(fd_set& writefds);

    std::atomic<bool> is_running_;
    std::vector<ConnFD> unknownConnect_;
    SocketFD sfd_;
    SocketAddrIn address_;
    IPAddress ipAddr_;
    Port port_;
    std::unordered_map<ConnFD, ClientInfo> clients_; 
    std::shared_ptr<std::thread> worker_;
};