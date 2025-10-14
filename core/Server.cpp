#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Server.h"
#include "utils/Utils.h"
#include "common/RetryOperation.h"



Server::Server(Port port) : sfd_(-1), is_running_(true) {
    port_ = port;
}

Server::~Server() {
    stop();
}

void Server::start() {
    int32_t max_sd;
    fd_set readfds, writefds;
    ConnFD new_conn;
    SocketAddrIn client_address;
    socklen_t client_addr_len = sizeof(client_address);

    std::pair<SocketFD, SocketAddrIn> fdAndAddr = createServerSocket();
    sfd_ = fdAndAddr.first;
    address_ = fdAndAddr.second;
    port_ = ntohs(address_.sin_port);
    ipAddr_ = inet_ntoa(address_.sin_addr);

    if (listen(sfd_, 3) < 0) {
        close(sfd_);
        throw std::runtime_error("Failed to listen on socket");
    }

    while(is_running_) {
        //Reinitialize the file descriptor set
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(sfd_, &readfds);
        max_sd = sfd_;
        for(const auto& [fd, cinfo] : clients_) {
            if (!cinfo.msg_queue.empty()) {
                FD_SET(fd, &writefds);
                max_sd = std::max(max_sd, fd);
            }
        }

        //Set timeout to 1 second for select
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        //Select for readability
        int activity = select(max_sd + 1, &readfds, &writefds, nullptr, &timeout);

        if ((activity < 0) && (errno != EINTR)) {
            LOG_ERROR("Select error");
        }

        //Check if there is a new connection
        if (FD_ISSET(sfd_, &readfds)) {
            if ((new_conn = accept(sfd_, (struct sockaddr*)&client_address, &client_addr_len)) < 0) {
                LOG_ERROR("Failed to accept connection");
                continue;
            }
            fcntl(new_conn, F_SETFL, fcntl(new_conn, F_GETFL, 0) | O_NONBLOCK);
            clients_[new_conn] = {new_conn, client_address, {}};

            LOG_INFO("New client connected: %d", new_conn);
        }

        //Check for IO operations on client sockets
        for(auto& [fd, cinfo] : clients_) {
            if (FD_ISSET(fd, &writefds) && cinfo.msg_queue.empty() == false) {
                std::string msg = cinfo.msg_queue.front();
                ssize_t bytes_sent = send(fd, msg.c_str(), msg.size(), 0);
                if (bytes_sent < 0) {
                    LOG_ERROR("Failed to send message to client %d", fd);
                } else {
                    LOG_INFO("Sent to client %d: %s", fd, msg);
                    cinfo.msg_queue.pop();
                }
            }
            if(is_running_ == false) break;
        }
    }
}

std::pair<SocketFD, SocketAddrIn> Server::createServerSocket(){
    auto socket_creator = [](int32_t retries_left) -> std::pair<SocketFD, SocketAddrIn> {
        int port = BASE_PORT + retries_left;
        SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sfd < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        SocketAddrIn address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        int opt = 1;
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(sfd, (struct sockaddr*)&address, sizeof(address)) == 0) {
            std::cout << "Successfully bound to port " << port << "\n";
            return {sfd, address};
        }

        close(sfd);
        throw std::runtime_error(std::string("Bind failed: "));
    };

    std::pair<SocketFD, SocketAddrIn> ret = retryOperation<std::pair<SocketFD, SocketAddrIn>>(socket_creator, 1000, 10);

    return ret;
}

void Server::stop() {
    if (isRunning() == false) {
        LOG_WARN("Client is not running");
        return;
    }

    setRunning(false);

    if (sfd_ != -1) {
        close(sfd_);
        sfd_ = -1;
    }

    for(auto& [fd, cinfo] : clients_) {
        close(fd);
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

IPAddress Server::getAddress() const {
    return ipAddr_;
}

Port Server::getPort() const {
    return port_;
}

std::vector<std::pair<ConnFD, std::string>> Server::getClientsList() const {
    std::vector<std::pair<ConnFD, std::string>> clients;
    for (const auto& [fd, cinfo] : clients_) {
        clients.emplace_back(fd, inet_ntoa(cinfo.address.sin_addr));
    }
    return clients;
}

void Server::sendToClient(ConnFD client_fd, const std::string& message) {
    try{
        ClientInfo& cinfo = clients_.at(client_fd);
        cinfo.msg_queue.push(message);
    } catch (const std::out_of_range& e) {
        LOG_ERROR("Client %d not found", client_fd);
    }
}