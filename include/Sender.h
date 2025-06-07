#define SENDER_H

#include <string>
#include <random>
#include "UDPSocket.h"
#include "Packet.h"

class Sender {
public:
    void run(const std::string& filePath, const std::string& destIP, int destPort);

private:

    UDPSocket socket;

    bool test(const std::string& destIP, int destPort);
    void sendFile(const std::string& filePath, const std::string& destIP, int destPort);
    bool sendDataPacketRDT(Packet pkt, const std::string& destIP, int destPort);
    
};