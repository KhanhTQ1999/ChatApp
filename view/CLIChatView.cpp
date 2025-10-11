#include <cassert>
#include "CLIChatView.h"

CLIChatView::CLIChatView(ChatViewModel& viewModel)
    : appState_(AppState::Running), viewModel_(viewModel){
    assert(&viewModel != nullptr);
    initializeUserOptions();
}

void CLIChatView::initializeUserOptions() {
    userOptions_ = {
        {1, {"1. help                               : Display user interface option\n", [this](){ viewModel_.handleHelpOption(); }}},
        {2, {"2. myip                               : Display IP adress of this app\n", [this](){ viewModel_.handleMyIPOption(); }}},
        {3, {"3. myport                             : Display listening port of this app\n", [this](){ viewModel_.handleMyPortOption(); }}},
        {4, {"4. connect <dest> <port>              : Connect to the app of another computer\n", [this](){ viewModel_.handleConnectOption(); }}},
        {5, {"5. list                               : List all connection of this app\n", [this](){ viewModel_.handleListOption(); }}},
        {6, {"6. terminate <conenction id>          : Terminate a connection\n", [this](){ viewModel_.handleTerminateOption(); }}},
        {7, {"7. send <connection id> <message>     : Send a message to a connection\n", [this](){ viewModel_.handleSendOption(); }}},
        {8, {"8. exit:                              : Close all connection & terminate this app\n", [this](){ stop(); }}}
    };
}

CLIChatView::~CLIChatView()
{
    //TODO: Cleanup if necessary
}

void CLIChatView::printUsage()
{
    std::cout << "********************Chat Application********************" << std::endl;
    std::cout << "Usage: chat-app [options]\n";
    std::cout << "Options:\n";
    for(const auto& opt : userOptions_){
        std::cout << opt.second.description;
    }
    std::cout << "**************************END**************************" << std::endl;
}

void CLIChatView::run()
{
    std::string line;

    while (appState_ == AppState::Running) {
        printUsage();
        std::cout << "Enter option: ";
        std::getline(std::cin, line);

        if (line.empty()) {
            std::cout << "No input detected. Please enter a number.\n";
            continue;
        }

        try {
            uint32_t opt = std::stoul(line);
            dispatchUserOpt(opt);
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

void CLIChatView::dispatchUserOpt(uint32_t opt)
{
    auto it = userOptions_.find(opt);
    if(it != userOptions_.end()){
        it->second.handler();
    } else {
        std::cout << "Invalid option. Type 'h' for help." << std::endl;
    }
}

void CLIChatView::stop()
{
    appState_ = AppState::Stopped;
    viewModel_.handleExitOption(); 
    std::cout << "Exiting application..." << std::endl;
}