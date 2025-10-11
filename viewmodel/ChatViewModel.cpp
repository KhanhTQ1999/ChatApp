#include "utils/Utils.h"
#include "viewmodel/ChatViewModel.h"

ChatViewModel::ChatViewModel(ChatModel& model) : model_(model) {
    //TODO: Initialization if necessary
}

ChatViewModel::~ChatViewModel()
{
    //TODO: Cleanup if necessary
}

void ChatViewModel::handleHelpOption() {
    LOG_INFO("Help option selected.");
    // Implement help option logic here
}

void ChatViewModel::handleMyIPOption() {
    LOG_INFO("My IP option selected.");
    // Implement my IP option logic here
}

void ChatViewModel::handleMyPortOption() {
    LOG_INFO("My Port option selected.");
    // Implement my port option logic here
}

void ChatViewModel::handleConnectOption() {
    LOG_INFO("Connect option selected.");
    // Implement connect option logic here
}

void ChatViewModel::handleListOption() {
    LOG_INFO("List option selected.");
    // Implement list option logic here
}

void ChatViewModel::handleTerminateOption() {
    LOG_INFO("Terminate option selected.");
    // Implement terminate option logic here
}

void ChatViewModel::handleSendOption() {
    LOG_INFO("Send option selected.");
    // Implement send option logic here
}

void ChatViewModel::handleExitOption() {
    LOG_INFO("Exit option selected.");
    // Implement exit option logic here
}

