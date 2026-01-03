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

        // Notify controller about user input
        context_.eventBus.emit("controller::user-input", input, std::vector<std::string>{});
    }
    return 0;
}

AppState CLI::getAppState() {
    return context_.appState;
}