#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <thread>
#include "core/Client.h"
#include "core/Server.h"

class ChatModel
{
public:
    ChatModel();
    virtual ~ChatModel();

    ChatModel(const ChatModel&) = default;
    ChatModel& operator=(const ChatModel&) = default;

    IPAddress getIPAddress() const;
    Port getPort() const;
    std::vector<std::pair<ConnFD, std::string>> listClients() const;
    void sendMessage(ConnFD fd, const std::string& message);
    void connectToServer(std::string addr, Port port);
    void endProgram();

private:
    Server server_;
    Client client_;
    std::thread serverThread_;
    std::thread clientThread_;
};

#endif // CHATMODEL_H