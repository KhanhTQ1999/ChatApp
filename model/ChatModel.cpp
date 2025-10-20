#include "model/ChatModel.h"

ChatModel::ChatModel() {
    server_.start();
    client_.start();
}

ChatModel::~ChatModel() {
    server_.stop();
    client_.stop();
}

IPAddress ChatModel::getIPAddress() const {
    return server_.getAddress();
}

Port ChatModel::getPort() const {
    return server_.getPort();
}

std::vector<std::pair<ConnFD, std::string>> ChatModel::listClients() const {
    return server_.getClientsList();
}

void ChatModel::sendMessage(ConnFD fd, const std::string& message) {
    server_.sendToClient(fd, message);
}

void ChatModel::connectToServer(std::string addr, Port port) {
    SocketFD sfd = client_.connectToServer(addr, port);
    if(sfd < 0) {
        throw std::runtime_error("Failed to connect to server");
    }

    std::string addrAndPort = getIPAddress() + ":" + std::to_string(getPort());
    int32_t ret = client_.sendToServer(sfd, "Hello from client at " + addrAndPort);
    if(ret < 0) {
        throw std::runtime_error("Failed to send message to server");
    }
}

void ChatModel::endProgram() {
    server_.stop();
    client_.stop();
}