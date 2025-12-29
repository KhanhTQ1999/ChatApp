#include "utils/Utils.h"
#include "views/ChatView.h"

ChatView::ChatView()
{
    cmndDesc_ = {
        "1. help                              : Display user interface options\n",
        "2. myip                              : Display IP address of this app\n",
        "3. myport                            : Display listening port of this app\n",
        "4. connect <dest> <port>             : Connect to the app of another computer\n",
        "5. list                              : List all connections of this app\n",
        "6. terminate <connection id>         : Terminate a connection\n",
        "7. send <connection id> <message>    : Send a message to a connection\n",
        "8. exit                              : Close all connections & terminate this app\n"
    };}

void ChatView::show(){
    if(getHideShowState()){
        LOG_INFO("ChatView is already visible");
        return;
    }
    setHideShowState(true);
    showMenu();

    while(getHideShowState()){
        listenUser();
    }
}

ChatView::~ChatView(){
    //Nothing to clean up
}

void ChatView::showMenu(){
    std::cout << "================== MENU ==================" << std::endl;
    for (const auto& desc : cmndDesc_) {
        std::cout << desc;
    }
    std::cout << "==========================================" << std::endl;
}

void ChatView::hide(){
    if(!getHideShowState()){
        LOG_INFO("ChatView is already hidden");
        return;
    }
    setHideShowState(false);
}

void ChatView::listenUser(){
    std::string input;
    std::cout << "Enter command: ";
    std::getline(std::cin, input);
    if(input == "exit"){
        hide();
        return;
    }
    //Notify controller about user input
}

void ChatView::setHideShowState(bool state){
    isShow_ = state;
}

bool ChatView::getHideShowState(){
    return isShow_;
}