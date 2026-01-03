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

void CLI::onShowError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
}

void CLI::onShowInfo(const std::string& infoMessage) {
    std::cout << "Info: " << infoMessage << std::endl;
}

int CLI::exec() {
    std::string input;
    while (true) {
        std::cout << "Enter command: ";
        std::getline(std::cin, input);
        if (input == "exit") {
            std::cout << "Exiting application. Goodbye!" << std::endl;
            break;
        }
        // Notify controller about user input
        context_.eventBus.emit("controller::user-input", input);
    }
    return 0;
}
