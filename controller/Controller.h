#pragma once

#include "common/typedef.h"
#include "model/ChatModel.h"
#include "views/ChatView.h"

class Controller {
public:
    Controller(AppContext& context, ChatModel& model, ChatView& view);
    ~Controller() = default;

    // Move semantics
    Controller(Controller&&) = default;
    Controller& operator=(Controller&&) = default;

    void dispatchUserCommand(const std::string cmd, const std::vector<std::string> args);
private:
    // Input parsing and dispatch
    std::pair<std::string, std::vector<std::string>> parseInput(std::string& input);
    bool validateUserInput(const std::string& input);

    // Command handlers (empty, to be implemented in .cpp)
    void handleHelpOption();
    void handleMyIPOption();
    void handleMyPortOption();
    void handleConnectOption(const std::vector<std::string>& args);
    void handleListOption();
    void handleTerminateOption(const std::vector<std::string>& args);
    void handleSendOption(const std::vector<std::string>& args);
    void handleExitOption();

    // Internal state
    AppContext& context_;
    AppState appState_{AppState::Stopped};
    ChatModel& model_;
    ChatView& view_;

    // Dispatcher map: command string -> handler
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> dispatcher_;
};
