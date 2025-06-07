#define RECEIVER_H

#include <fstream>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string.h>

#include "UDPSocket.h"
#include "Packet.h"
#include "Utils.h"

class Receiver {
public:
    void run(const std::string& outputFile, int listenPort, double dataDropProb = 0.0);

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