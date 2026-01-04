#pragma once

#include "common/typedef.h"

class NetworkService
{
public:
    NetworkService(AppContext& context);
    ~NetworkService();

    NetworkService(NetworkService&&) = default;
    NetworkService& operator=(NetworkService&&) = default;

    std::pair<int, std::string> startServer(const std::string ipAddress, int startPort);
    std::pair<int, std::string> stopServer();
    std::pair<int, std::string> connectToPeer(std::string ipAddress, int port);
    std::pair<int, std::string> disconnectFromPeer(int sfdToDisconnect);
    std::pair<int, std::string> sendMessageToPeer(int sfdToSend, const std::string& message);
    std::vector<int> getActiveConnections();

private:
    int bindSocket(int& sfd, const std::string& ipAddress, const int& startPort);
    int closeSocket(int& sfd);
    void updateServerInfo(const std::string& ipAddress, int port);
    AppState getAppState();

    AppContext& context_;
    std::vector<int> cfdList_; //Client list. Use for receiving messages from peers
    std::vector<int> sfdList_; //Server list. Use for sending messages to peers
};