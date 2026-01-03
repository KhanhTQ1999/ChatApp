#pragma once

#include "common/typedef.h"
#include "views/IView.h"

// Forward declaration
class ChatModel;

class ChatView : public IView {
public:
    ChatView(AppContext& context);
    ~ChatView();

    ChatView(ChatView&&) = default;
    ChatView& operator=(ChatView&&) = default;

    void show() override;
    void hide() override;
    void setHideShowState(bool state);
    bool getHideShowState();
    
    // Connect to model for observing changes
    void connectToModel(ChatModel& model);
    
    // Display methods
    void displayConnections();
    void displayMessages();
    void displayIPInfo(const std::string& ip, int port);

private:
    void showMenu();

    AppContext& context_;
    ChatModel* model_ = nullptr;
    std::vector<ChatOption> chatOption_;
    bool isShow_{false};
};
