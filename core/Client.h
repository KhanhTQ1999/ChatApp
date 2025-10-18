#pragma one

#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common/typedef.h"

typedef struct {
    ConnFD fd;
    std::string address;
    Port port;
} ServerInfo;

class Client {
public:
    Client();
    virtual ~Client();

    Client(const Client&) = default;
    Client& operator=(const Client&) = default;

    void start();
    void stop();
    int32_t multiplex();
    void listenServer(SocketFD sfd);
    std::vector<ServerInfo> getSInfoList() const;
    void setRunning(bool running);
    bool isRunning() const;
    SocketFD connectToServer(std::string addr, Port port);
    int32_t sendToServer(SocketFD sfd, const std::string& message);

private:
    bool is_running_;
    std::vector<ServerInfo> sinfo_list_;
};




// #pragma once

// #include <vector>
// #include <string>
// #include <memory>
// #include <atomic>
// #include <thread>
// #include <mutex>
// #include <optional>
// #include <functional>
// #include <algorithm>
// #include <chrono>
// #include <iostream>
// #include <unistd.h>
// #include <sys/select.h>
// #include <sys/socket.h>
// #include <fcntl.h>
// #include <cstring>

// // Type alias
// using SocketFD = int;

// struct ServerInfo {
//     SocketFD fd;
//     std::string name;
// };

// class Client {
// public:
//     Client();
//     ~Client();

//     void start();
//     void stop();
//     void addServer(SocketFD fd, const std::string& name);

// private:
//     void runLoop();
//     int32_t multiplex();
//     void listenServer(SocketFD sfd);

//     SocketFD prepareFdSet(fd_set& readfds);
//     timeval getSelectTimeout() const;

//     std::vector<ServerInfo> sinfo_list_;
//     std::atomic<bool> running_{false};
//     std::unique_ptr<std::thread> worker_;
//     std::mutex sinfo_mutex_;
// };