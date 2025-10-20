#pragma one

#include <iostream>
#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <thread>
#include <shared_mutex>
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
    void listenMultiplex(SocketFD maxfd, fd_set& readfds, struct timeval& timeout);
    void listenServer(SocketFD sfd);
    std::vector<ServerInfo> getSInfoList() const;
    void setRunning(bool running);
    bool isRunning() const;
    SocketFD connectToServer(std::string addr, Port port);
    int32_t sendToServer(SocketFD sfd, const std::string& message);
    void cleanup();

private:
    void runLoop();
    std::shared_ptr<ServerInfo> findServer(SocketFD sfd) const;
    SocketFD createSocket() const;
    void configureSocket(SocketFD sfd) const;
    SocketAddrIn buildAddress(const std::string& addr, Port port) const;
    void initiateConnection(SocketFD sfd, const sockaddr_in& svaddr) const;
    struct timeval getSelectTimeout() const;
    void handleReadySockets(const fd_set& readfds);
    SocketFD initializeFdSet(fd_set& readfds);
    void addServer(ServerInfo sinfo);
    void removeServer(SocketFD sfd);
    void onMessageReceived(SocketFD sfd, const std::string& message);

    mutable std::shared_mutex sinfo_mutex_;
    std::atomic<bool> is_running_;
    std::vector<std::shared_ptr<ServerInfo>> sinfo_list_;
    std::shared_ptr<std::thread> worker_;
};