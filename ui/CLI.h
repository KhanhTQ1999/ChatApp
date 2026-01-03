#pragma once

#include "common/typedef.h"
#include "UI.h"
class CLI : public UI
{
public:
    CLI(AppContext& context);
    ~CLI() override;
    void onChatView_ShowMainMenu(const std::vector<ChatOption>& options) override;
    void onShowError(const std::string& errorMessage) override;
    void onShowInfo(const std::string& infoMessage) override;
    int exec() override;

private:
    AppContext& context_;
};
