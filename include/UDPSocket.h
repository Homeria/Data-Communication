#define UDP_SOCKET_H

#include <string>
#include <vector>
#include <netinet/in.h>

class UDPSocket {
public:
    UDPSocket();
    ~UDPSocket();

    bool bindSocket(int port);
    bool sendTo(const std::vector<uint8_t>& data, const std::string& destIP, int destPort);
    bool receiveFrom(std::vector<uint8_t>& buffer, std::string& senderIP, int& senderPort);

    void setRecvTimeout(int seconds);

private:
    int sockfd;
    struct sockaddr_in addr;
};
