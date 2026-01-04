#include "utils/Utils.h"
#include "views/ChatView.h"

ChatView::ChatView(AppContext& context)
    : context_(context)
{
    chatOption_ = {
        {"help", " - Display this help menu\n"},
        {"myip", " - Display your IP address\n"},
        {"myport", " - Display your listening port\n"},
        {"connect <ip> <port>", " - Connect to a peer\n"},
        {"list", " - List all connected peers\n"},
        {"terminate <connection id>", " - Terminate a connection\n"},
        {"send <connection id> <message>", " - Send a message to a peer\n"},
        {"exit", " - Exit the application\n"}
    };
}

void ChatView::show(){
    if(getHideShowState()){
        LOG_INFO("ChatView is already visible");
        return;
    }
    setHideShowState(true);
    context_.eventBus.emit("ui::show-chat-menu", chatOption_);
}

ChatView::~ChatView(){
    //Nothing to clean up
}

void ChatView::hide(){
    if(!getHideShowState()){
        LOG_INFO("ChatView is already hidden");
        return;
    }
    setHideShowState(false);
    context_.eventBus.emit("ui::hide-chat-menu", chatOption_);
}

void ChatView::setHideShowState(bool state){
    isShow_ = state;
}

bool ChatView::getHideShowState(){
    return isShow_;
}

void ChatView::displayConnections(const std::vector<int>& connectionIds) {
    context_.eventBus.emit("ui::show-all-connections", connectionIds);
}