#pragma once

#include "common/typedef.h"

class NetworkService
{
public:
    NetworkService(AppContext& context);
    ~NetworkService();

    NetworkService(NetworkService&&) = default;
    NetworkService& operator=(NetworkService&&) = default;

    std::pair<int, std::string> startServer();
    std::pair<int, std::string> stopServer();

private:
    AppContext& context_;
};