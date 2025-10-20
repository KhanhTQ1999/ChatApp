#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "core/Client.h"
#include "utils/Utils.h"
#include "common/TryCatch.h"

Client::Client() : sinfo_list_(), is_running_(false), worker_(nullptr) {
}

Client::~Client() {
    stop();
}

void Client::start() {
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

void Client::runLoop(){
    while(is_running_.load()) {
        pattern::tryCatch("Client multiplexing", [this]() {
            fd_set readfds;
            SocketFD maxfd = initializeFdSet(readfds);
            struct timeval timeout = getSelectTimeout();
            listenMultiplex(maxfd, readfds, timeout);
        });
        usleep(100 * 1000);
    }
}

void Client::listenMultiplex(SocketFD maxfd, fd_set& readfds, struct timeval& timeout) {
    //Select for readability
    int32_t ready = select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);
    if(ready < 0) {
        throw std::runtime_error("Select error");
    }

    // Check if any sockets are ready
    if(ready > 0 && is_running_.load()) {
        // Some sockets are ready
        handleReadySockets(readfds);
        return;
    }
}

SocketFD Client::initializeFdSet(fd_set& readfds) {
    FD_ZERO(&readfds);
    SocketFD maxfd = -1;

    std::shared_lock lock(sinfo_mutex_);
    for (const auto &sinfo : sinfo_list_) {
        FD_SET(sinfo->fd, &readfds);
        if (sinfo->fd > maxfd) {
            maxfd = sinfo->fd;
        }
    }
    return maxfd;
}

struct timeval Client::getSelectTimeout() const {
    return {0, 100000}; // 100 milliseconds
}

void Client::handleReadySockets(const fd_set& readfds) {
    std::shared_lock lock(sinfo_mutex_);
    for (const auto &sinfo : sinfo_list_) {
        if (FD_ISSET(sinfo->fd, &readfds)) {
            listenServer(sinfo->fd);
        }
    }
}

void Client::listenServer(SocketFD sfd) {
    char buffer[1024];
    auto [ret, error] = pattern::tryCatchWithPair("Read from server", [sfd, &buffer]() {
        ssize_t n = read(sfd, buffer, sizeof(buffer) - 1);
        if(n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            throw std::runtime_error("Read error");
        }
        if(n == 0) {
            throw std::runtime_error("Connection closed by server");
        }
        return n;
    });

    if(error) {
        LOG_ERROR("%s", error->c_str());
        close(sfd);
        removeServer(sfd);
        return;
    }
    
    ssize_t n = *ret;
    buffer[n] = '\0';
    onMessageReceived(sfd, std::string(buffer));
}

void Client::onMessageReceived(SocketFD sfd, const std::string& message) {
    LOG_INFO("Received from server (fd=%d): %s", sfd, message.c_str());
    // Handle the received message as needed
}

void Client::stop() {
    //Check if already stopped
    if (!is_running_.exchange(false)) {
        return;
    }
    cleanup();
}

void Client::cleanup(){
    //Wait for worker thread to finish
    if (worker_ && worker_->joinable()) {
        worker_->join();
        worker_.reset();
    }

    //Close all server sockets
    std::unique_lock lock(sinfo_mutex_);
    for (const auto &sinfo : sinfo_list_) {
        close(sinfo->fd);
    }
    sinfo_list_.clear();
}

bool Client::isRunning() const {
    return is_running_.load();
}

void Client::setRunning(bool running) {
    is_running_.store(running);
}

SocketFD Client::connectToServer(std::string addr, Port port) {

    SocketFD sfd = createSocket();
    SocketAddrIn svaddr = buildAddress(addr, port);
    configureSocket(sfd);
    initiateConnection(sfd, svaddr);
    addServer({sfd, addr, port});
    LOG_INFO("Connected to server");

    return sfd;
}

SocketFD Client::createSocket() const {
    // Create socket
    SocketFD sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    return sfd;
}

void Client::configureSocket(SocketFD sfd) const {
    // Set socket to non-blocking
    int flags = fcntl(sfd, F_GETFL, 0);
    if(fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(sfd);
        throw std::runtime_error("Failed to set socket to non-blocking");
    }
}

SocketAddrIn Client::buildAddress(const std::string& addr, Port port) const {
    SocketAddrIn svaddr;
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, addr.c_str(), &svaddr.sin_addr) <= 0){
        throw std::runtime_error("Invalid address/ Address not supported");
    }
    return svaddr;
}

void Client::initiateConnection(SocketFD sfd, const sockaddr_in& svaddr) const {
    if (connect(sfd, reinterpret_cast<const sockaddr*>(&svaddr), sizeof(svaddr)) < 0) {
        if (errno != EINPROGRESS) {
            close(sfd);
            throw std::runtime_error("Failed to connect to server");
        }
    }
}

void Client::addServer(ServerInfo sinfo) {
    std::unique_lock lock(sinfo_mutex_);
    sinfo_list_.push_back(std::make_shared<ServerInfo>(sinfo));
}

int32_t Client::sendToServer(SocketFD sfd, const std::string& message) {
    auto sinfo = findServer(sfd);
    if (!sinfo) {
        LOG_WARN("SocketFD %d not found in server list", sfd);
        return -1;
    }

    ssize_t bytes_sent = send(sfd, message.c_str(), message.size(), 0);
    if (bytes_sent < 0) {
        LOG_ERROR("Failed to send message to server (fd=%d)", sfd);
        return -1;
    }

    LOG_INFO("Sent to server (fd=%d): %s", sfd, message.c_str());
    return static_cast<int32_t>(bytes_sent);
}

std::shared_ptr<ServerInfo> Client::findServer(SocketFD sfd) const {
    std::shared_lock lock(sinfo_mutex_);
    for (const auto& sinfo : sinfo_list_) {
        if (sinfo && sinfo->fd == sfd) {
            return sinfo;
        }
    }
    return nullptr;
}

void Client::removeServer(SocketFD sfd) {
    std::unique_lock lock(sinfo_mutex_);
    sinfo_list_.erase(
        std::remove_if(sinfo_list_.begin(), sinfo_list_.end(),
            [sfd](const std::shared_ptr<ServerInfo>& sinfo) {
                return sinfo && sinfo->fd == sfd;
            }),
        sinfo_list_.end()
    );
}

std::vector<ServerInfo> Client::getSInfoList() const {
    std::shared_lock lock(sinfo_mutex_);
    std::vector<ServerInfo> result(sinfo_list_.size());
    for (const auto& sinfo : sinfo_list_) {
        if (sinfo) {
            result.push_back(*sinfo); // Dereference shared_ptr to get ServerInfo
        }
    }
    return result;
}