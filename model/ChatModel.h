#pragma once

class ChatModel
{
public:
    ChatModel() = default;
    ~ChatModel() = default;

    ChatModel(ChatModel&&) = default;
    ChatModel& operator=(ChatModel&&) = default;
};