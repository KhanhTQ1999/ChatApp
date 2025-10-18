#include <sstream>
#include <cassert>
#include "CLIChatView.h"
#include "common/TryCatch.h"

CLIChatView::CLIChatView(ChatViewModel& viewModel)
    : appState_(AppState::Running), viewModel_(viewModel){
    assert(&viewModel != nullptr);
    initializeUserOptions();
    resignSubscriber();
}

void CLIChatView::initializeUserOptions() {
    cmdInfo_ = {
        {"help", [this](const std::vector<std::string>& args) {
            viewModel_.handleHelpOption();
        }},
        {"myip", [this](const std::vector<std::string>& args) {
            viewModel_.handleMyIPOption();
        }},
        {"myport", [this](const std::vector<std::string>& args) {
            viewModel_.handleMyPortOption();
        }},
        {"connect", [this](const std::vector<std::string>& args) {
            viewModel_.handleConnectOption(args);
        }},
        {"list", [this](const std::vector<std::string>& args) {
            viewModel_.handleListOption();
        }},
        {"terminate", [this](const std::vector<std::string>& args) {
            viewModel_.handleTerminateOption(args);
        }},
        {"send", [this](const std::vector<std::string>& args) {
            viewModel_.handleSendOption(args);
        }},
        {"exit", [this](const std::vector<std::string>& args) {
            stop();
        }}
    };

    cmndDesc_ = {
        "1. help                              : Display user interface options\n",
        "2. myip                              : Display IP address of this app\n",
        "3. myport                            : Display listening port of this app\n",
        "4. connect <dest> <port>             : Connect to the app of another computer\n",
        "5. list                              : List all connections of this app\n",
        "6. terminate <connection id>         : Terminate a connection\n",
        "7. send <connection id> <message>    : Send a message to a connection\n",
        "8. exit                              : Close all connections & terminate this app\n"
    };
}

void CLIChatView::resignSubscriber()
{
    viewModel_.subcrible([this](const Message& msg){
        std::string timeStr = std::ctime(&msg.timestamp);
        timeStr.pop_back();  // remove trailing '\n'
        std::cout << "[" << timeStr << "] " << msg.sender << ": " << msg.content << std::endl;
    });
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
    for(const auto& desc : cmndDesc_){
        std::cout << desc;
    }
    std::cout << "**************************END**************************" << std::endl;
}

void CLIChatView::run()
{
    std::string line;
    printUsage();
    
    while (appState_ == AppState::Running) {
        //Get user input
        auto [ret, error] = pattern::tryCatchWithTuple("CLIChatView::run", [&]() {
            line = getUserInput();
            validateUserInput(line);
            auto [command, args] = parseInput(line);
            dispatchUserOpt(command, args);
        });
    }
}

std::string CLIChatView::getUserInput() {
    std::cout << "Enter option: ";
    std::string line;
    std::getline(std::cin, line);
    return line;
}


void CLIChatView::validateUserInput(std::string& input)
{
    if(input.empty()) {
        throw std::runtime_error("Input cannot be empty.");
    }
}

std::pair<std::string, std::vector<std::string>> CLIChatView::parseInput(std::string& input)
{
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

void CLIChatView::dispatchUserOpt(std::string cmd, std::vector<std::string> args)
{
    auto it = cmdInfo_.find(cmd);
    if(it != cmdInfo_.end()){
        it->second(args);
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