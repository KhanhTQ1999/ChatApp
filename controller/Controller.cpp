#include "common/typedef.h"
#include "utils/Utils.h"
#include "controller/Controller.h"

Controller::Controller(AppContext& context, ChatModel& model, ChatView& view)
    : context_(context), model_(model), view_(view), appState_(AppState::Stopped)
{
    dispatcher_ = {
        {"help",      [this](const std::vector<std::string>&) { handleHelpOption(); }},
        {"myip",      [this](const std::vector<std::string>&) { handleMyIPOption(); }},
        {"myport",    [this](const std::vector<std::string>&) { handleMyPortOption(); }},
        {"connect",   [this](const std::vector<std::string>& args) { handleConnectOption(args); }},
        {"list",      [this](const std::vector<std::string>&) { handleListOption(); }},
        {"terminate", [this](const std::vector<std::string>& args) { handleTerminateOption(args); }},
        {"send",      [this](const std::vector<std::string>& args) { handleSendOption(args); }},
        {"exit",      [this](const std::vector<std::string>&) { handleExitOption(); }}
    };
};

bool Controller::validateUserInput(const std::string& input) {
    return !input.empty();
}

std::pair<std::string, std::vector<std::string>> Controller::parseInput(std::string& input) {
    std::istringstream ss(input);
    std::string command;
    ss >> command;

    std::vector<std::string> args;
    std::string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }

    return {command, args};
}

void Controller::dispatchUserCommand(const std::string& cmd, const std::vector<std::string>& args) {
    auto it = dispatcher_.find(cmd);
    if (it != dispatcher_.end()) {
        it->second(args);
    } else {
        std::cout << "Invalid command. Type 'help' for available options." << std::endl;
    }
}

// Show help menu to the user
void Controller::handleHelpOption() {
    // TODO: Implement logic to display available commands
    LOG_INFO("Help option selected");
}

// Show the current IP address of the application
void Controller::handleMyIPOption() {
    LOG_INFO("MyIP option selected");
    context_.eventBus.emit("ui::show-info", ("Current IP: " + context_.serverInfo.ip).c_str());
}

// Show the current port number of the application
void Controller::handleMyPortOption() {
    LOG_INFO("MyPort option selected");
    context_.eventBus.emit("ui::show-info", ("Current Port: " + std::to_string(context_.serverInfo.port)).c_str());
}

// Connect to a peer using host and port arguments
void Controller::handleConnectOption(const std::vector<std::string>& args) {
    if(args.size() != 2){
        LOG_ERROR("Connect command requires 2 arguments: <host> <port>");
        context_.eventBus.emit("ui::show-error", "Connect command requires 2 arguments: <host> <port>");
        return;
    }

    std::string host = args[0];
    int port = std::stoi(args[1]);

    context_.eventBus.emit("network::connect-to-peer", host, port);
}

// List all active peer connections
void Controller::handleListOption() {
    LOG_INFO("List all connections");
    context_.eventBus.emit("network::list-active-connections");
}

// Terminate a connection by its ID
void Controller::handleTerminateOption(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        LOG_ERROR("Terminate command requires 1 arguments: <connection_id>");
        context_.eventBus.emit("ui::show-error", "Terminate command requires 1 arguments: <connection_id>");
        return;
    }

    int connectionId = std::stoi(args[0]);
    context_.eventBus.emit("network::disconnect-from-peer", connectionId);
}

// Send a message to a peer by connection ID
void Controller::handleSendOption(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        LOG_ERROR("Send command requires 2 arguments: <connection_id> <message>");
        context_.eventBus.emit("ui::show-error", "Send command requires 2 arguments: <connection_id> <message>");
        return;
    }

    int connectionId = std::stoi(args[0]);
    std::string message = args[1];

    context_.eventBus.emit("network::send-to-peer", connectionId, message);
}

// Exit the application gracefully
void Controller::handleExitOption() {
    LOG_INFO("Exiting application as per user command.");
    context_.appState = AppState::Stopped;
}
