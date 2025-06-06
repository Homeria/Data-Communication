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

        std::cout << "[Send] " << pkt.toString() << std::endl;

        alarm(TIMEOUT_TIME);

        std::vector<uint8_t> ackBuffer;
        std::string senderIP;
        int senderPort;

        bool ackReceived = false;

        while (true) {

            ackReceived = socket.receiveFrom(ackBuffer, senderIP, senderPort);
            bool isDropped = shouldDrop(PACKET_DROP);
            if (isDropped) {
                std::cerr << "[DROPPED] AckPacket" << std::endl;
                continue;
            } else {
                alarm(0);
                break;
            }
        }

        if (!ackReceived) {

            std::cerr << "[TIMEOUT] Ack not received for seqNum=" << pkt.seqNum << std::endl;
            alarm(0);
            continue;

        }

        Packet ackPkt = Packet::deserialize(ackBuffer);

        std::cout << "[Receive] " << ackPkt.toString() << "\n" << std::endl;
        break;

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
        std::cerr << "File open failed!" << std::endl;
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
            std::cout << "[ERROR] The Packet not sent" << std::endl;
            continue;
        }
        
    }

    
    
    Packet pktEOT(EOT, seqNum);
    std::cout << "[Send] " << pktEOT.toString() << std::endl;
    socket.sendTo(pktEOT.serialize(), destIP, destPort);

    inFile.close();

    std::cout << "Sending File and \"Finish\" is succeed." << std::endl;

}