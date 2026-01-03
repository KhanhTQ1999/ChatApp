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

void Controller::dispatchUserCommand(const std::string cmd, const std::vector<std::string> args) {
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
    // TODO: Implement logic to retrieve and display local IP
    LOG_INFO("MyIP option selected");
}

// Show the current port number of the application
void Controller::handleMyPortOption() {
    // TODO: Implement logic to retrieve and display local port
    LOG_INFO("MyPort option selected");
}

// Connect to a peer using host and port arguments
void Controller::handleConnectOption(const std::vector<std::string>& args) {
    // TODO: Implement logic to connect to a peer
    LOG_INFO("Connect to peer");
    for(const auto& arg : args){
        LOG_INFO("Arg: %s", arg.c_str());
    }
}

// List all active peer connections
void Controller::handleListOption() {
    // TODO: Implement logic to list all current connections
    LOG_INFO("List all connections");

}

// Terminate a connection by its ID
void Controller::handleTerminateOption(const std::vector<std::string>& args) {
    // TODO: Implement logic to terminate a connection
    LOG_INFO("Terminate connection id");
    for(const auto& arg : args){
        LOG_INFO("Arg: %s", arg.c_str());
    }
}

// Send a message to a peer by connection ID
void Controller::handleSendOption(const std::vector<std::string>& args) {
    // TODO: Implement logic
    LOG_INFO("Sending message to connection id");
    for(const auto& arg : args){
        LOG_INFO("Arg: %s", arg.c_str());
    }
}

void Controller::handleExitOption() {
    // TODO: Implement logic
    LOG_INFO("Exiting application as per user command.");
}
