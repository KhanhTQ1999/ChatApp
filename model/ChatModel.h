#pragma once

#include "common/typedef.h"

class ChatModel
{
public:
    ChatModel(AppContext& context);
    ~ChatModel();

    ChatModel(ChatModel&&) = default;
    ChatModel& operator=(ChatModel&&) = default;
};