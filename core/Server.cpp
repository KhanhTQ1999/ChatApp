#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include "Server.h"
#include "utils/Utils.h"

Server::Server(IPAddress addr, Port port) : sfd_(-1), is_running_(true) {
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = htonl(addr);
    address_.sin_port = htons(port);
}

Server::~Server() {
    stop();
}

void Server::start() {
    int32_t new_conn, max_sd;;
    SocketAddrIn client_address;
    socklen_t client_addr_len = sizeof(client_address);

    sfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    if (bind(sfd_, (struct sockaddr*)&address_, sizeof(address_)) < 0) {
        close(sfd_);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(sfd_, 3) < 0) {
        close(sfd_);
        throw std::runtime_error("Failed to listen on socket");
    }

    LOG_INFO("Server started on port %d", ntohs(address_.sin_port));

    while(is_running_) {
        //Reinitialize the file descriptor set
        FD_ZERO(&readfds_);
        FD_SET(sfd_, &readfds_);
        max_sd = sfd_;
        for (int sd : clients_) {
            FD_SET(sd, &readfds_);
            max_sd = std::max(max_sd, sd);
        }

        //Set timeout to 1 second for select
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        //Select for readability
        int activity = select(max_sd + 1, &readfds_, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            LOG_ERROR("Select error");
        }

        //Check if there is a new connection
        if (FD_ISSET(sfd_, &readfds_)) {
            if ((new_conn = accept(sfd_, (struct sockaddr*)&client_address, &client_addr_len)) < 0) {
                LOG_ERROR("Failed to accept connection");
                continue;
            }
            fcntl(new_conn, F_SETFL, fcntl(new_conn, F_GETFL, 0) | O_NONBLOCK);
            clients_.push_back(new_conn);
            LOG_INFO("New client connected: %d", new_conn);
        }

        //Check for IO operations on client sockets
        for (int32_t client_fd : clients_) {
            if (FD_ISSET(client_fd, &readfds_)) {
                listenClient(client_fd);
                if(is_running_ == false) break;
            }
        }

    }
}

void Server::listenClient(int32_t client_fd) {
    char buffer[1024] = {0};
    int32_t valread = read(client_fd, buffer, 1024);
    if (valread > 0) {
        std::string message(buffer, valread);
        LOG_INFO("Received from client %d: %s", client_fd, message);
    } else if (valread == 0) {
        LOG_INFO("Client %d disconnected", client_fd);
        close(client_fd);
        for(auto it = clients_.begin(); it != clients_.end(); ++it) {
            if(*it == client_fd) {
                clients_.erase(it);
                break;
            }
        }
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERROR("Failed to read from client %d", client_fd);
            close(client_fd);
        }
    }
}

void Server::stop() {
    setRunning(false);

    if (sfd_ != -1) {
        close(sfd_);
        sfd_ = -1;
    }

    for (int32_t client_fd : clients_) {
        close(client_fd);
    }
    clients_.clear();

    LOG_INFO("Server stopped");
}

void Server::setRunning(bool running) {
    is_running_ = running;
}

bool Server::isRunning() const {
    return is_running_;
}