#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <functional>
#include "model/ChatModel.h"
#include "common/typedef.h"
#include "common/Observer.h"

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
    void handleConnectOption(std::vector<std::string> args);
    void handleListOption();
    void handleTerminateOption(std::vector<std::string> args);
    void handleSendOption(std::vector<std::string> args);
    void handleExitOption();
    void subcrible(std::function<void(const Message&)> callback);

private:
    ChatModel& model_;
    base::Observer<Message> latestMessage_;
};
#endif // CHATVIEWMODEL_H