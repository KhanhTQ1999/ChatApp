#pragma once

#include "common/typedef.h"

class NetworkService
{
public:
    NetworkService(AppContext& context);
    ~NetworkService();

    NetworkService(NetworkService&&) = default;
    NetworkService& operator=(NetworkService&&) = default;

    std::pair<int, std::string> startServer(const std::string& ipAddress, int startPort);
    std::pair<int, std::string> stopServer();
    std::pair<int, std::string> connectToPeer(const std::string& ipAddress, int port);
    std::pair<int, std::string> disconnectFromPeer(const std::string& ipAddress, int port);
    std::pair<int, std::string> sendMessage(const std::string& ipAddress, int port, const std::string& message);

private:
    int BindSocket(int& sfd, const int& startPort);
    int CloseSocket(int& sfd);
    AppState getAppState();

    AppContext& context_;
};