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

std::pair<int, std::string> NetworkService::startServer(const std::string& ipAddress, int startPort)
{
    struct sockaddr_in addr;
    int sfd, cfd;
	char send_buff[1025];
    ssize_t numRead;

    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sfd < 0){
        LOG_ERROR("Failed to create socket");
        return {-1, "Failed to create socket"};
    }

    if(BindSocket(sfd, ipAddress, startPort) < 0){
        LOG_ERROR("Failed to bind socket");
        CloseSocket(sfd);
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
                CloseSocket(sfd);
                return {-1, "Failed to accept connection"};
            }
        }
		/* As soon as server gets a request from client, it prepares the date and time and
		 * writes on the client socket through the descriptor returned by accept()
		 */
		snprintf(send_buff, sizeof(send_buff), "Hello client from server\n");
	}
    return {0, ""};
}

int NetworkService::BindSocket(int& sfd, const std::string& ipAddress, const int& startPort)
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

int NetworkService::CloseSocket(int& sfd)
{
    if (sfd >= 0) {
        close(sfd);
        sfd = -1;
    }
    return 0;
}

std::pair<int, std::string> NetworkService::stopServer()
{
}

AppState NetworkService::getAppState()
{
    return context_.appState;
}