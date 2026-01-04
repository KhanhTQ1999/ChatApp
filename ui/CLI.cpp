#include "CLI.h"

CLI::CLI(AppContext& context)
    : context_(context)
{
}

CLI::~CLI()
{
}

void CLI::onChatView_ShowMainMenu(const std::vector<ChatOption>& options) {
    std::cout << "Welcome to the Chat Application (CLI Version)!" << std::endl;
    std::cout << "Type 'help' to see available commands." << std::endl;
    std::cout << "\n================== MENU ==================" << std::endl;
    for (const auto& option : options) {
        std::cout << option.action << option.description;
    }
    std::cout << "==========================================" << std::endl;
}

void CLI::onChatView_ShowAllConnections(const std::vector<int>& connectionIds) {
    if(connectionIds.empty()) {
        std::cout << "No active connections." << std::endl;
        return;
    }

    std::cout << "Active Connections:" << std::endl;
    for (const auto& id : connectionIds) {
        std::cout << "Connection ID: " << id << std::endl;
    }
}

void CLI::onShowError(const char* errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
}

void CLI::onShowInfo(const char* infoMessage) {
    std::cout << "Info: " << infoMessage << std::endl;
}

int CLI::exec() {
    std::string input;
    while (getAppState() == AppState::Running) {
        std::cout << "Enter command: ";
        std::getline(std::cin, input);

        auto [command, args] = parseInput(input);
        // Notify controller about user input
        context_.eventBus.emit("controller::user-input", command, args);
    }
    return 0;
}

AppState CLI::getAppState() {
    return context_.appState;
}

std::pair<std::string, std::vector<std::string>> CLI::parseInput(const std::string& input) {
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