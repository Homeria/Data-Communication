#define UDP_SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <unistd.h>   

#include <cstring>
#include <vector>
#include <iostream> 

#include "Packet.h"


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
