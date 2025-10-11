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
    void printUsage();
    void dispatchUserOpt(uint32_t opt);


    std::unordered_map <uint32_t, UserOption> userOptions_;
    AppState appState_;
    ChatViewModel& viewModel_;
};

#endif // CLIChatView_H