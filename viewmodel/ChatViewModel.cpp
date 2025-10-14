#include "utils/Utils.h"
#include "viewmodel/ChatViewModel.h"

ChatViewModel::ChatViewModel(ChatModel& model) : model_(model) {
    //TODO: Initialization if necessary
}

ChatViewModel::~ChatViewModel()
{
    //TODO: Cleanup if necessary
}

void ChatViewModel::handleHelpOption() {
    LOG_INFO("Help option selected.");
    // Implement help option logic here
}

void ChatViewModel::handleMyIPOption() {
    LOG_INFO("My IP option selected.");
    IPAddress ip = model_.getIPAddress();
    latestMessage_.set({
        .sender = "System",
        .content = ip,
        .timestamp = std::time(nullptr)
    });
}

void ChatViewModel::handleMyPortOption() {
    LOG_INFO("My Port option selected.");
    Port port = model_.getPort();
    latestMessage_.set({
        .sender = "System",
        .content = std::to_string(port),
        .timestamp = std::time(nullptr)
    });
}

void ChatViewModel::handleConnectOption(std::vector<std::string> args) {
    LOG_INFO("Connect option selected.");
    if(args.size() < 2) {
        latestMessage_.set({
            .sender = "System",
            .content = "Error: Not enough arguments for connect command.",
            .timestamp = std::time(nullptr)
        });
        return;
    }

    try{
        std::string ip = args[0];
        Port port = static_cast<Port>(std::stoul(args[1]));
        model_.connectToServer(ip, port);
    }catch(const std::exception& e){
        latestMessage_.set({
            .sender = "System",
            .content = "Error: Invalid IP address or port.",
            .timestamp = std::time(nullptr)
        });
        return;
    }
}

void ChatViewModel::handleListOption() {
    LOG_INFO("List option selected.");

    std::vector<std::pair<ConnFD, std::string>> clients = model_.listClients();
    std::string clientList;
    for (const auto& client : clients) {
        clientList += std::to_string(client.first) + ": " + client.second + "\n";
    }
    latestMessage_.set({
        .sender = "System",
        .content = clientList.empty() ? "No clients connected." : clientList,
        .timestamp = std::time(nullptr)
    });
}

void ChatViewModel::handleTerminateOption(std::vector<std::string> args) {
    LOG_INFO("Terminate option selected.");
}

void ChatViewModel::handleSendOption(std::vector<std::string> args) {
    LOG_INFO("Send option selected.");
    if(args.size() < 2) {
        latestMessage_.set({
            .sender = "System",
            .content = "Error: Not enough arguments for send command.",
            .timestamp = std::time(nullptr)
        });
        return;
    }

    try{
        ConnFD conn_id = std::stoul(args[0]);
        std::string message = args[1];
        model_.sendMessage(conn_id, message);
    }catch(const std::exception& e){
        latestMessage_.set({
            .sender = "System",
            .content = "Error: Invalid connection id.",
            .timestamp = std::time(nullptr)
        });
        return;
    }
}

void ChatViewModel::handleExitOption() {
    LOG_INFO("Exit option selected.");
    model_.endProgram();
}

void ChatViewModel::subcrible(std::function<void(const Message&)> callback) {
    latestMessage_.subscribe(callback);
}
