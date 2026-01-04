#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/RetryOperation.h"
#include "utils/Utils.h"
#include "services/NetworkService.h"

#define BACKLOG 5
#define BUF_SIZE 1024
#define SV_SOCK_PATH "/tmp/server_socket"

NetworkService::NetworkService(AppContext& context)
    : context_(context)
{
    // Initialize network service with context if needed
}
NetworkService::~NetworkService()
{
    // Cleanup resources if needed
}

std::pair<int, std::string> NetworkService::startServer(const std::string ipAddress, int startPort)
{
    struct sockaddr_in addr;
    int sfd, cfd;
	char recv_buff[1024];
    ssize_t numRead;

    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sfd < 0){
        LOG_ERROR("Failed to create socket");
        return {-1, "Failed to create socket"};
    }

    if(bindSocket(sfd, ipAddress, startPort) < 0){
        LOG_ERROR("Failed to bind socket");
        closeSocket(sfd);
        return {-1, "Failed to bind socket"};
    }

    if(listen(sfd, BACKLOG) < 0){
        LOG_ERROR("Failed to listen on socket");
        return {-1, "Failed to listen on socket"};
    }

    context_.eventBus.emit("ui::show-info", ("Server started on port " + context_.serverInfo.ip + ":" + std::to_string(context_.serverInfo.port)).c_str());

    while(getAppState() == AppState::Running)
	{
		/* In the call to accept(), the server is put to sleep and when for an incoming
		 * client request, the three way TCP handshake* is complete, the function accept()
		 * wakes up and returns the socket descriptor representing the client socket.
		 */
		cfd = accept(sfd, (struct sockaddr*)NULL, NULL);
        if(cfd < 0){
            if(errno == EWOULDBLOCK || errno == EAGAIN){
                // No incoming connection, continue the loop
                usleep(100); // Sleep for 100 microseconds to avoid busy waiting
                continue;
            } else {
                LOG_ERROR("Failed to accept connection");
                closeSocket(sfd);
                return {-1, "Failed to accept connection"};
            }
        }else{
            cfdList_.push_back(cfd);
            LOG_INFO("Accepted new connection, cfd: %d", cfd);
        }
		/* As soon as server gets a request from client, it prepares the date and time and
		 * writes on the client socket through the descriptor returned by accept()
		 */
        for(int fd : cfdList_){
            numRead = read(fd, recv_buff, BUF_SIZE);
            if(numRead > 0){
                LOG_INFO("Received message from cfd %d: %s", fd, recv_buff);
            }
        }
	}
    return {0, ""};
}

int NetworkService::bindSocket(int& sfd, const std::string& ipAddress, const int& startPort)
{
    int ret = pattern::retryOperation<int>([this, &sfd, &ipAddress, &startPort](int32_t retriesLeft) {
        int ret;
        int port = startPort + (5 - retriesLeft);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port); // Try next port if in use

        ret = bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
        if (ret < 0) {
            LOG_WARN("Bind failed on port %d, retries left: %d", ntohs(addr.sin_port), retriesLeft);
            throw std::runtime_error("Bind failed");
        }
        this->updateServerInfo(ipAddress, port);
        LOG_INFO("Successfully bound to port %d", ntohs(addr.sin_port));

        return ret;
    }, 1000, 5);

    return ret;
}

void NetworkService::updateServerInfo(const std::string& ipAddress, int port) {
    context_.serverInfo.ip = ipAddress;
    context_.serverInfo.port = port;
}

int NetworkService::closeSocket(int& sfd)
{
    if (sfd >= 0) {
        close(sfd);
        sfd = -1;
    }
    return 0;
}

AppState NetworkService::getAppState()
{
    return context_.appState;
}

std::pair<int, std::string> NetworkService::connectToPeer(std::string ipAddress, int port)
{
    int sfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;

    /* a socket is created through call to socket() function */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		LOG_ERROR("Could not create socket");
		return {-1, "Could not create socket"};
	}

    memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

    /* Convert IPv4 and IPv6 addresses from text to binary form */
	if(inet_pton(AF_INET, ipAddress.c_str(), &serv_addr.sin_addr)<=0)
	{
		LOG_ERROR("inet_pton error occurred");
		return {-1, "inet_pton error occurred"};
	}

	/* Information like IP address of the remote host and its port is
	 * bundled up in a structure and a call to function connect() is made
	 * which tries to connect this socket with the socket (IP address and port)
	 * of the remote host
	 */
	if(connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		LOG_ERROR("Connect failed");
		return {-1, "Connect failed"};
	}

    sfdList_.push_back(sfd);
    LOG_INFO("Successfully connected to peer %s:%d", ipAddress.c_str(), port);
    return {sfd, "Connected to peer " + ipAddress + ":" + std::to_string(port)};
}

std::pair<int, std::string> NetworkService::disconnectFromPeer(int sfdToDisconnect)
{
    auto it = std::find(sfdList_.begin(), sfdList_.end(), sfdToDisconnect);
    if (it == sfdList_.end()) {
        LOG_WARN("No active connection found for peer %d", sfdToDisconnect);
        return {-1, "No active connection found"};
    }

    closeSocket(sfdToDisconnect);
    sfdList_.erase(it);
    LOG_INFO("Disconnected from peer %d", sfdToDisconnect);
    return {0, ""};
}

std::pair<int, std::string> NetworkService::sendMessageToPeer(int sfdToSend, const std::string& message)
{
    auto it = std::find(sfdList_.begin(), sfdList_.end(), sfdToSend);
    if(it == sfdList_.end()){
        LOG_WARN("No active connection found for peer %d", sfdToSend);
        return {-1, "No active connection found"};
    }

    ssize_t bytesSent = send(sfdToSend, message.c_str(), message.size(), 0);
    if(bytesSent < 0){
        LOG_ERROR("Failed to send message to %d", sfdToSend);
        return {-1, "Failed to send message"};
    }else{
        LOG_INFO("Sent message to %d", sfdToSend);
        return {0, "Sent message to " + std::to_string(sfdToSend)};
    }
}

std::vector<int> NetworkService::getActiveConnections()
{
    return sfdList_;
}

