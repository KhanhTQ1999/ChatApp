#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "core/Client.h"
#include "utils/Utils.h"

Client::Client() : sinfo_list_(), is_running_(false) {
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (isRunning()) {
        LOG_WARN("Client is already running");
        return;
    }

    int32_t sleepMs = 100;
    setRunning(true);

    while(isRunning()) {
        int32_t ret = multiplex();
        if(ret < 0) {
            LOG_ERROR("Client multiplex error");
            break;
        }
        sleepMs = ret > 0 ? 100 : 0;
        usleep(sleepMs * 1000);
    }

    LOG_INFO("Client started");
}

int32_t Client::multiplex() {
    fd_set readfds;
    SocketFD maxfd = -1;
    FD_ZERO(&readfds);
    for (ServerInfo sinfo : sinfo_list_) {
        FD_SET(sinfo.fd, &readfds);
        maxfd = std::max(maxfd, sinfo.fd);
    }

    //Set timeout to 1 second for select
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    //Select for readability
    int32_t ready = select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);
    if (ready < 0) {
        LOG_ERROR("Select error");
        return -1;
    } else if (ready == 0) {
        // Timeout, no sockets are ready
        return 0;
    }else if(isRunning()) {
        // Some sockets are ready
        for (ServerInfo sinfo : sinfo_list_) {
            if (FD_ISSET(sinfo.fd, &readfds)) {
                listenServer(sinfo.fd);
            }
        }
    }
    return ready;
}

void Client::listenServer(SocketFD sfd) {
    char buffer[1024];
    int n = read(sfd, buffer, sizeof(buffer));
    if (n > 0) {
        buffer[n] = '\0';
        LOG_INFO("Received from server: %s\n", buffer);
    } else if (n == 0) {
        LOG_INFO("Server closed connection");
        close(sfd);
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERROR("Read error");
            close(sfd);
        }
    }
}

void Client::stop() {
    if (isRunning() == false) {
        return;
    }
    setRunning(false);

    for (ServerInfo sinfo : sinfo_list_) {
        close(sinfo.fd);
    }
    sinfo_list_.clear();
}

bool Client::isRunning() const {
    return is_running_;
}

void Client::setRunning(bool running) {
    is_running_ = running;
}

SocketFD Client::connectToServer(std::string addr, Port port) {
    // Create socket
    SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Configure server address structure
    struct sockaddr_in svaddr;
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(port);
    
    // Set socket to non-blocking
    int flags = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags | O_NONBLOCK);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, addr.c_str(), &svaddr.sin_addr) <= 0){
        close(sfd);
        throw std::runtime_error("Invalid address/ Address not supported");
    }

    // Connect to server
    if (connect(sfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) < 0) {
        if (errno != EINPROGRESS) {
            close(sfd);
            throw std::runtime_error("Failed to connect to server");
        }
    }

    sinfo_list_.push_back({sfd, addr, port});
    LOG_INFO("Connected to server");

    return sfd;
}

int32_t Client::sendToServer(SocketFD sfd, const std::string& message) {
    ssize_t bytes_sent = -1;
    // Check if sfd is in the list
    for(const auto& sinfo : sinfo_list_) {
        if (sinfo.fd == sfd) {
            // Send message to server
            ssize_t bytes_sent = send(sfd, message.c_str(), message.size(), 0);
            if (bytes_sent < 0) {
                LOG_ERROR("Failed to send message to server");
                return -1;
            }

            LOG_INFO("Sent to server: %s", message.c_str());
            break;
        }
    }
    // If not found, return error
    LOG_ERROR("Socket FD not found in server list");
    return -1;


    return bytes_sent;
}

std::vector<ServerInfo> Client::getSInfoList() const {
    return sinfo_list_;
}