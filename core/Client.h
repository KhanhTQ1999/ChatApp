#pragma one

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
    bool isRunning() const;
    void connectToServer(const SocketAddrIn& svaddr);

private:
    bool is_running_;
    std::vector<SocketFD> sfd_list_;
};