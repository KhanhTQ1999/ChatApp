#include "utils/Utils.h"
#include "viewmodel/ChatViewModel.h"
#include "common/TryCatch.h"

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
    notifyObservers(ip);
}

void ChatViewModel::handleMyPortOption() {
    LOG_INFO("My Port option selected.");
    Port port = model_.getPort();
    notifyObservers(std::to_string(port));
}

void ChatViewModel::handleConnectOption(std::vector<std::string> args) {
    LOG_INFO("Connect option selected.");

    auto error = pattern::tryCatchWithError("Connect to server", [this, &args]() {
        if(args.size() < 2) {
            throw std::runtime_error("Not enough arguments for connect command.");
        }
        std::string ip = args[0];
        Port port = static_cast<Port>(std::stoul(args[1]));
        model_.connectToServer(ip, port);
    });

    if(error) {
        notifyObservers(*error);
    }
    return;
}

void ChatViewModel::handleListOption() {
    LOG_INFO("List option selected.");

    std::vector<std::pair<ConnFD, std::string>> clients = model_.listClients();
    std::string clientList;
    for (const auto& client : clients) {
        clientList += std::to_string(client.first) + ": " + client.second + "\n";
    }
    notifyObservers(clientList);
}

void ChatViewModel::handleTerminateOption(std::vector<std::string> args) {
    LOG_INFO("Terminate option selected.");
}

void ChatViewModel::handleSendOption(std::vector<std::string> args) {
    LOG_INFO("Send option selected.");

    auto error = pattern::tryCatchWithError("Send message", [this, &args]() {
        if(args.size() < 2) {
            throw std::runtime_error("Not enough arguments for send command.");
        }
        ConnFD conn_id = static_cast<ConnFD>(std::stoul(args[0]));
        std::string message = args[1];
        model_.sendMessage(conn_id, message);
    });

    if(error) {
        notifyObservers(*error);
    }
    return;
}

void ChatViewModel::notifyObservers(std::string message, std::time_t timestamp) {
    latestMessage_.set({
        .sender = "System",
        .content = message,
        .timestamp = timestamp
    });
}
void ChatViewModel::handleExitOption() {
    LOG_INFO("Exit option selected.");
    model_.endProgram();
}

void ChatViewModel::subcrible(std::function<void(const Message&)> callback) {
    latestMessage_.subscribe(callback);
}
