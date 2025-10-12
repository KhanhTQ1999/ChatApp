#ifndef CHATMODEL_H
#define CHATMODEL_H

#include "core/Client.h"
#include "core/Server.h"

class ChatModel
{
public:
    explicit ChatModel();
    virtual ~ChatModel();

    ChatModel(const ChatModel&) = default;
    ChatModel& operator=(const ChatModel&) = default;

private:
    Server server_;
    Client client_;
};

#endif // CHATMODEL_H