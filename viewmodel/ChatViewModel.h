#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include "model/ChatModel.h"

class ChatViewModel
{
public:
    explicit ChatViewModel(ChatModel& model);
    virtual ~ChatViewModel();

    ChatViewModel(const ChatViewModel&) = default;
    ChatViewModel& operator=(const ChatViewModel&) = default;

    void handleHelpOption();
    void handleMyIPOption();
    void handleMyPortOption();
    void handleConnectOption();
    void handleListOption();
    void handleTerminateOption();
    void handleSendOption();
    void handleExitOption();

private:
    ChatModel& model_;
};
#endif // CHATVIEWMODEL_H