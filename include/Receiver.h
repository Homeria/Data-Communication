#define RECEIVER_H

#include <string>
#include "UDPSocket.h"
#include "Packet.h"

class Receiver {
public:
    void run(const std::string& outputFile, int listenPort);

private:
    UDPSocket socket;
    std::string senderIP;
    int senderPort;

    bool test();

    bool bindAndWait(int listenPort);

    void receiveFile(const std::string& outputFile);

    Packet receiveDataPacketRDT(int seqNum, std::string& senderIP, int& senderPort);
    void sendAckPacket(int ackNum, std::string senderIP, int senderPort);
    void sendEOTAckPacket(int seqNum, std::string senderIP, int senderPort);

};