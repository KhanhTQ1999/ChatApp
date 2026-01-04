#pragma once

#include "common/typedef.h"
#include "UI.h"
class CLI : public UI
{
public:
    CLI(AppContext& context);
    ~CLI() override;
    void onChatView_ShowMainMenu(const std::vector<ChatOption>& options) override;
    void onChatView_ShowAllConnections(const std::vector<int>& connectionIds) override;
    void onShowError(const char* errorMessage) override;
    void onShowInfo(const char* infoMessage) override;
    int exec() override;

private:
    AppState getAppState();
    std::pair<std::string, std::vector<std::string>> parseInput(const std::string& input);

    AppContext& context_;
};
