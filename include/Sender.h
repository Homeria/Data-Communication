#define SENDER_H

#include "UDPSocket.h"
#include "Packet.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <string.h>

class Sender {
public:
    void run(const std::string& filePath, const std::string& destIP, int destPort, double ackDropProb = 0.0, int timeout = 2);

private:

    UDPSocket socket;

    bool test(const std::string& destIP, int destPort);
    void sendFile(const std::string& filePath, const std::string& destIP, int destPort);
    bool sendDataPacketRDT(Packet pkt, const std::string& destIP, int destPort);
    
};