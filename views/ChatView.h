#pragma once

#include "common/typedef.h"
#include "views/IView.h"

class ChatView : public IView {
public:
    ChatView();
    ~ChatView();

    ChatView(ChatView&&) = default;
    ChatView& operator=(ChatView&&) = default;

    void show() override;
    void hide() override;
    void listenUser();
    void setHideShowState(bool state);
    bool getHideShowState();

private:
    void showMenu();

    std::unordered_map <std::string, std::function<void(std::vector<std::string> args)>> cmdInfo_;
    std::vector<std::string> cmndDesc_;
    bool isShow_{true};
};