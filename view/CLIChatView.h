#ifndef CLIChatView_H
#define CLIChatView_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

#include "IChatView.h"
#include "viewmodel/ChatViewModel.h"

enum class AppState : uint32_t {
    Running,
    Stopped
};

typedef struct{
    std::string description;
    std::function<void()> handler;
} UserOption;

class CLIChatView : public IChatView {
public:
    CLIChatView(ChatViewModel& viewModel);
    ~CLIChatView();

    CLIChatView(CLIChatView&&) = default;
    CLIChatView& operator=(CLIChatView&&) = default;

    void run() override;
    void stop() override;

private:
    void initializeUserOptions();
    void resignSubscriber();
    void printUsage();
    void dispatchUserOpt(std::string cmd, std::vector<std::string> args);


    std::unordered_map <std::string, std::function<void(std::vector<std::string> args)>> cmdInfo_;
    std::vector<std::string> cmndDesc_;
    AppState appState_;
    ChatViewModel& viewModel_;
};

#endif // CLIChatView_H