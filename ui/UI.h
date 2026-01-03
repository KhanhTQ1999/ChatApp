#pragma once

// UI type enum for factory pattern
enum class UIType {
    CLI,
    QT
};

// Abstract base class for UI
class UI
{
public:
    virtual void onChatView_ShowMainMenu(const std::vector<ChatOption>& options) = 0;
    virtual void onShowError(const char* errorMessage) = 0;
    virtual void onShowInfo(const char* infoMessage) = 0;
    virtual int exec() = 0;
    virtual ~UI() {}
};
