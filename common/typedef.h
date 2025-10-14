#pragma once

#include <string>
#include <ctime>
#include <cstdint>

typedef int32_t SocketFD;
typedef int32_t ConnFD;
typedef std::string IPAddress;
typedef uint16_t Port;
typedef struct sockaddr_in SocketAddrIn;

typedef struct {
    std::string sender;
    std::string content;
    std::time_t timestamp;
} Message;