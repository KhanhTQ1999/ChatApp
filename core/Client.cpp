#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "core/Client.h"
#include "utils/Utils.h"

Client::Client() : sfd_list_(), is_running_(false) {
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (is_running_) {
        LOG_WARN("Client is already running");
        return;
    }
    setRunning(true);
    fd_set readfds;
    while(is_running_){
        //Reinitialize the file descriptor set
        SocketFD maxfd = -1;
        FD_ZERO(&readfds);
        for (SocketFD sfd : sfd_list_) {
            FD_SET(sfd, &readfds);
            maxfd = std::max(maxfd, sfd);
        }

        //Set timeout to 1 second for select
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        //Select for readability
        int32_t activity = select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);
        if (activity < 0) {
            LOG_ERROR("Select error");
            break;
        }

        //Check for readability on each socket
        for (int32_t sfd : sfd_list_) {
            if (FD_ISSET(sfd, &readfds)) {
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
            if(is_running_ == false) break;
        }
    }

    LOG_INFO("Client started");
}

void Client::stop() {
    if (isRunning() == false) {
        LOG_WARN("Client is not running");
        return;
    }

    setRunning(false);

    for (SocketFD sfd : sfd_list_) {
        close(sfd);
    }
    sfd_list_.clear();

    LOG_INFO("Client stopped");
}

bool Client::isRunning() const {
    return is_running_;
}

void Client::setRunning(bool running) {
    is_running_ = running;
}

SocketFD Client::connectToServer(std::string addr, Port port) {
    struct sockaddr_in svaddr;

    SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags | O_NONBLOCK);

    if (sfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr.c_str(), &svaddr.sin_addr) <= 0){
        close(sfd);
        throw std::runtime_error("Invalid address/ Address not supported");
    }

    if (connect(sfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) < 0) {
        if (errno != EINPROGRESS) {
            close(sfd);
            throw std::runtime_error("Failed to connect to server");
        }
    }

    sfd_list_.push_back(sfd);
    LOG_INFO("Connected to server");

    return sfd;
}

int32_t Client::sendToServer(SocketFD sfd, const std::string& message) {
    if(sfd_list_.end() == std::find(sfd_list_.begin(), sfd_list_.end(), sfd)) {
        LOG_ERROR("Socket FD not found in client list");
        return -1;
    }
    ssize_t bytes_sent = send(sfd, message.c_str(), message.size(), 0);
    if (bytes_sent < 0) {
        LOG_ERROR("Failed to send message to server");
        return -1;
    }
    LOG_INFO("Sent to server: %s", message.c_str());
    return bytes_sent;
}