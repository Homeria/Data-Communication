#include "UDPSocket.h"
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <cstring>      
#include <iostream>     
#include "Packet.h"

#define BUFFER_SIZE 4096 

/**
 * @brief Structor: Create a UDP socket
 */
UDPSocket::UDPSocket() {

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Print error if socket creation is failed
    if (sockfd < 0) {
        perror("Socket Creation Failed.");
    }

    // Memory Reset
    memset(&addr, 0, sizeof(addr));
}

/**
 * @brief Destructor: close the socket 
 */
UDPSocket::~UDPSocket() {
    // Properly close the socket
    close(sockfd);
}

/**
 * @brief Bind the socket to a specific port for receiving data
 * @param port port number
 * @return true if bind is succesfully done, else false.
 */
bool UDPSocket::bindSocket(int port) {

    addr.sin_family = AF_INET;

    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr)) <= 0) {
        perror("Invalid address or not supported.");
        return false;
    }

    if (bind(sockfd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return false;
    }

    return true;
}

/**
 * @brief Send a message to a specified destination IP and port
 * @param message message
 * @param destIP destination IP Address
 * @param destPort destination Port Number
 */
bool UDPSocket::sendTo(const std::vector<uint8_t>& data, const std::string& destIP, int destPort) {
    
    // 
    struct sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));

    // Use IPv4
    destAddr.sin_family = AF_INET;

    // Set destination port
    destAddr.sin_port = htons(destPort);

    
    if (inet_pton(AF_INET, destIP.c_str(), &destAddr.sin_addr) <= 0) {
        perror("Invalid destination IP address");
        return false;
    }

    /*
        Send the message from destIP:destPort using UDP.

        ssize_t sentBytes = sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr*)&destAddr, sizeof(destAddr));
            sockfd : Socket (UDP)
            message.c_str() : Data to send (C-style string)
            message.size() : Data size to send
            0 : flag (Normally UDP -> 0)
            destAddr : Destination IP address
            sizeof(destAddr) : Address structure size
    */
    ssize_t sentBytes = sendto(sockfd, data.data(), data.size(), 0,
                               (const struct sockaddr*)&destAddr, sizeof(destAddr));
    
    // Return true if message was sent successfully.
    return sentBytes >= 0;
}


/**
 * @brief Receive a message and retrieve sender's IP and port
 * @param message message
 * @param senderIP Sender IP address
 * @param senderPort Sender Port Number
 */
bool UDPSocket::receiveFrom(std::vector<uint8_t>& buffer, std::string& senderIP, int& senderPort) {

    struct sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);
    uint8_t recvBuffer[1024];

    ssize_t bytesReceived = recvfrom(sockfd, recvBuffer, sizeof(recvBuffer), 0,
                                     (struct sockaddr*)&senderAddr, &addrLen);

    if (bytesReceived < 0) {
        if (errno == EINTR) {
            // std::cerr << "[TIMEOUT] recvfrom interrupted by signal (EINTR)" << std::endl;
            return false; // 타임아웃 상황
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // std::cerr << "[TIMEOUT] recvfrom() timed out" << std::endl;
        } else {
            // std::cerr << "[ERROR] recvfrom() failed" << std::endl;
        }
        return false;
    }

    buffer.assign(recvBuffer, recvBuffer + bytesReceived);
    senderIP = inet_ntoa(senderAddr.sin_addr);
    senderPort = ntohs(senderAddr.sin_port);

    return true;
}
