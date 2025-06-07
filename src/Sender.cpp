#include "Sender.h"
#include "Packet.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <string.h>

#define PACKET_DROP 0.1
#define TIMEOUT_TIME 2

#define EOT_RETRANSMISSION 5


void sigalrm_handler(int sig) {
    // std::cerr << "[TIMEOUT] Alarming!" << std::endl;
}

void set_sigalrm_handler() {
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // SA_RESTART를 끄는 것이 중요!
    sigaction(SIGALRM, &sa, NULL);
}

void Sender::run(const std::string& filePath, const std::string& destIP, int destPort) {

    
    sendFile(filePath, destIP, destPort);

}

bool Sender::test(const std::string& destIP, int destPort) {

    Packet pkt(DATA, 0, "Test! ABCD!");
    std::cout << pkt.toString() << std::endl;
    socket.sendTo(pkt.serialize(), destIP, destPort);

    return true;
}

bool Sender::sendDataPacketRDT(Packet pkt, const std::string& destIP, int destPort) {

    std::cout << "\n[Send Data Packet] " << "================" << std::endl;


    set_sigalrm_handler();

    while (true) {

        socket.sendTo(pkt.serialize(), destIP, destPort);

        printLog(pkt, "SEND DATA");

        alarm(TIMEOUT_TIME);

        std::vector<uint8_t> ackBuffer;
        std::string senderIP;
        int senderPort;
        Packet ackPkt;

        bool ackReceived = socket.receiveFrom(ackBuffer, senderIP, senderPort);
        bool isDropped = shouldDrop(PACKET_DROP);

        if (ackReceived) {

            ackPkt = Packet::deserialize(ackBuffer);
            
            if (isDropped) {
                printError(pkt, "DROPPED ACK");
                ackReceived = socket.receiveFrom(ackBuffer, senderIP, senderPort);
            } else {
                alarm(0);
                printLog(ackPkt, "RECEIVE ACK");
                break;
                
            }
        } else {
            printError(pkt, "TIMEOUT DATA");
            alarm(0);
            continue;
        }

    }

    return true;
}

/**
 * ender -> receiver : file, text-"Finish"
 * @param filePath fileName
 * @param destIP destination IP address
 * @param destPort destination Port Number
 */
void Sender::sendFile(const std::string& filePath, const std::string& destIP, int destPort) {

    // if opening file is failed
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        printError(Packet(), "File open failed");
        return;
    }

    char buffer[50];
    int seqNum = 0;

    // 파일을 끝까지 읽을 때까지 반복

    while (inFile.read(buffer, sizeof(buffer)) || inFile.gcount() > 0) {

        std::streamsize bytesRead = inFile.gcount();

        Packet pkt(DATA, seqNum, std::string(buffer, bytesRead));

        bool isSent = sendDataPacketRDT(pkt, destIP, destPort);
        if (isSent) {
            seqNum++;
        } else {
            printError(pkt, "The Packet not sent");
            continue;
        }
        
    }

    int count = 0;
    while (count < EOT_RETRANSMISSION) {
        
        count++;

        Packet pktEOT(EOT, seqNum);

        bool isSent = sendDataPacketRDT(pktEOT, destIP, destPort);
        if (isSent) {
            std::cout << "Sending File and \"Finish\" is succeed." << std::endl;
            printLog(pktEOT, "SEND EOT");
            break;
        } else {
            continue;
        }
    }
    
    inFile.close();

}