#ifndef CHATMODEL_H
#define CHATMODEL_H

#include "core/NetworkManager.h"

class ChatModel
{
public:
    explicit ChatModel(NetworkManager& networkManager);
    virtual ~ChatModel();

    ChatModel(const ChatModel&) = default;
    ChatModel& operator=(const ChatModel&) = default;

private:
    NetworkManager& networkManager_;
};

#endif // CHATMODEL_H