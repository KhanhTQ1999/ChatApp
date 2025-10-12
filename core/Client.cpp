#include <unistd.h>
#include <fcntl.h>

#include "core/Client.h"
#include "utils/Utils.h"

Client::Client() : sfd_list_(-1), is_running_(false) {
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (is_running_) {
        LOG_WARN("Client is already running");
        return;
    }

    fd_set readfds;
    while(is_running_){
        //Reinitialize the file descriptor set
        FD_ZERO(&readfds);
        for (SocketFD sfd : sfd_list_) {
            FD_SET(sfd, &readfds);
        }
        SocketFD maxfd = sfd_list_.empty() ? -1 : sfd_list_.size();

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
    is_running_ = false;

    if (!is_running_) {
        LOG_WARN("Client is not running");
        return;
    }

    for (SocketFD sfd : sfd_list_) {
        close(sfd);
    }
    sfd_list_.clear();

    LOG_INFO("Client stopped");
}

void Client::connectToServer(const SocketAddrIn& svaddr) {
    SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags | O_NONBLOCK);

    if (sfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    if (connect(sfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) < 0) {
        close(sfd);
        throw std::runtime_error("Failed to connect to server");
    }

    sfd_list_.push_back(sfd);

    LOG_INFO("Connected to server");
}

bool Client::isRunning() const {
    return is_running_;
}
