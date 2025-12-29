#pragma once

#include "common/typedef.h"
#include "model/ChatModel.h"
#include "views/IView.h"

// Application state enum
enum class AppState : uint32_t {
    Running,
    Stopped
};

class Controller {
public:
    Controller(ChatModel& model, IView& view);
    ~Controller() = default;

    // Move semantics
    Controller(Controller&&) = default;
    Controller& operator=(Controller&&) = default;

    // Main loop control
    void start();
    void stop();
    bool isRunning() const;

private:
    void runLoop();

    // Input parsing and dispatch
    std::pair<std::string, std::vector<std::string>> parseInput(std::string& input);
    void dispatchUserCommand(const std::string cmd, const std::vector<std::string> args);
    bool validateUserInput(const std::string& input);

    // Command handlers (empty, to be implemented in .cpp)
    void handleHelpOption();
    void handleMyIPOption();
    void handleMyPortOption();
    void handleConnectOption(const std::vector<std::string>& args);
    void handleListOption();
    void handleTerminateOption(const std::vector<std::string>& args);
    void handleSendOption(const std::vector<std::string>& args);

    // Internal state
    AppState appState_{AppState::Stopped};
    ChatModel& model_;
    IView& view_;

    // Dispatcher map: command string -> handler
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> dispatcher_;
};