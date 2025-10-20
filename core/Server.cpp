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
    //Check if already running
    if (is_running_.exchange(true)) {
        LOG_WARN("Client is already running");
        return;
    }

    //Start worker thread
    worker_ = std::make_shared<std::thread>([this]() {
        runLoop();
        stop();
    });
}

void Server::runLoop() {
    int32_t opt = 1;
    int32_t max_sd;
    fd_set readfds, writefds;
    
    SocketAddrIn client_address;
    socklen_t client_addr_len = sizeof(client_address);


    sfd_ = createSocket();
    setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    std::pair<SocketFD, SocketAddrIn> fdAndAddr = bindAddress();

    address_ = fdAndAddr.second;
    port_ = ntohs(address_.sin_port);
    ipAddr_ = inet_ntoa(address_.sin_addr);

    if (listen(sfd_, 3) < 0) {
        close(sfd_);
        throw std::runtime_error("Failed to listen on socket");
    }

    while(is_running_) {
        //Reinitialize the file descriptor set
        SocketFD read_max_sd = initializeReadFds(readfds);
        SocketFD write_max_sd = initializeWriteFds(writefds);
        max_sd = std::max(read_max_sd, write_max_sd);

        //Set timeout to 1 second for select
        struct timeval timeout = getSelectTimeout();

        // Communicate client activity
        multiplexClients(max_sd, readfds, writefds, timeout);
    }
}

void Server::multiplexClients(SocketFD max_sd, fd_set& readfds, fd_set& writefds, struct timeval& timeout) {
    //Select for readability and writability
    int32_t ready = select(max_sd + 1, &readfds, &writefds, nullptr, &timeout);
    if(ready < 0) {
        throw std::runtime_error("Select error");
    }

    // Check if any sockets are ready
    if(ready > 0 && is_running_) {
        //Check for new connections
        if (FD_ISSET(sfd_, &readfds)) {
            ConnFD new_conn; = accept(sfd_, (struct sockaddr*)&client_address, &client_addr_len);
            if (new_conn < 0) {
                LOG_ERROR("Accept failed");
            } else {
                //Add new client
                clients_.emplace(new_conn, ClientInfo{new_conn, client_address, std::queue<std::string>()});
                LOG_INFO("New client connected: %s", inet_ntoa(client_address.sin_addr));
            }
        }

        //Check existing clients for readability
        for (auto it = clients_.begin(); it != clients_.end(); ) {
            ConnFD client_fd = it->first;
            ClientInfo& cinfo = it->second;

            //Handle readable client sockets
            if (FD_ISSET(client_fd, &readfds)) {
                listenClient(client_fd);
            }

            //Handle writable client sockets
            if (FD_ISSET(client_fd, &writefds)) {
                while (!cinfo.msg_queue.empty()) {
                    const std::string& msg = cinfo.msg_queue.front();
                    ssize_t bytes_sent = send(client_fd, msg.c_str(), msg.size(), 0);
                    if (bytes_sent < 0) {
                        LOG_ERROR("Failed to send message to client (fd=%d)", client_fd);
                        break;
                    }
                    cinfo.msg_queue.pop();
                }
            }
            ++it;
        }
    }
}

SocketFD Server::initializeReadFds(fd_set& readfds) {
    FD_ZERO(&readfds);
    SocketFD maxfd = sfd_;

    //Add server socket
    FD_SET(sfd_, &readfds);

    //Add client sockets
    for (const auto& [fd, cinfo] : clients_) {
        FD_SET(fd, &readfds);
        maxfd = std::max(maxfd, fd);
    }

    return maxfd;
}

SocketFD Server::initializeWriteFds(fd_set& writefds) {
    SocketFD max_sd = -1;
    FD_ZERO(&writefds);

    for(const auto& [fd, cinfo] : clients_) {
        if (!cinfo.msg_queue.empty()) {
            FD_SET(fd, &writefds);
            max_sd = std::max(max_sd, fd);
        }
    }
    return max_sd;
}

struct timeval Server::getSelectTimeout() const {
    return {0, 100000}; // 1 second
}

std::pair<SocketFD, SocketAddrIn> Server::bindAddress() const{

    auto binder = [this](int32_t retries_left) -> std::pair<SocketFD, SocketAddrIn> {
        int32_t opt = 1;
        int port = BASE_PORT + retries_left;
        SocketFD sfd = getSfd();
        SocketAddrIn svaddr = buildAddress(INADDR_ANY, port);
        
        if (bind(sfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) < 0) {
            close(sfd);
            throw std::runtime_error("Bind failed");
        }
        return {sfd, svaddr};
    };

    return pattern::retryOperation<std::pair<SocketFD, SocketAddrIn>>(binder, 1000, 10);
}

SocketFD Server::createSocket() const {
    SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    return sfd;
}

SocketAddrIn Server::buildAddress(const std::string& addr, Port port) const {
    SocketAddrIn svaddr;
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = INADDR_ANY;
    svaddr.sin_port = htons(port);
    return svaddr;
}

void Server::stop() {
    //Check if already stopped
    if (!is_running_.exchange(false)) {
        return;
    }
    cleanup();
}

void Server::cleanup(){
    //Wait for worker thread to finish
    if (worker_ && worker_->joinable()) {
        worker_->join();
        worker_.reset();
    }

    if (sfd_ != -1) {
        close(sfd_);
        sfd_ = -1;
    }

    for(auto& [fd, cinfo] : clients_) {
        close(fd);
    }
    clients_.clear();
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

SocketFD Server::getSfd() const {
    return sfd_;
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