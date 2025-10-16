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