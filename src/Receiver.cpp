#include "Receiver.h"
#include "Packet.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string.h>

#define PACKET_DROP 0.05
#define TIMEOUT_TIME 2

#define EOT_ACK_TIME 5


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


void Receiver::run(const std::string& outputFile, int listenPort) {

    
    
    std::cout << "[START] 0. Set and Bind socket ===========================<<" << std::endl;
    if (!bindAndWait(listenPort)) {
        return;
    } else {
        std::cout << "instruct and bind socket successfully." << std::endl;
    }
    std::cout << "[END] 0. Set and Bind socket ===========================\n" << std::endl;
    std::cout << "[Test] wait the file" << std::endl;
    receiveFile(outputFile);

}

bool Receiver::bindAndWait(int listenPort) {
    if (!socket.bindSocket(listenPort)) {
        std::cerr << "Failed to bind on port " << listenPort << std::endl;
        return false;
    }
    return true;
}

bool Receiver::test() {
    std::vector<uint8_t> buffer;
    socket.receiveFrom(buffer, senderIP, senderPort);
    Packet pkt = Packet::deserialize(buffer);
    std::cout << pkt.toString() << std::endl;
    
    return true;
}

Packet Receiver::receiveDataPacketRDT(int seqNum, std::string& senderIP, int& senderPort) {
    
    std::cout << "\n[Receive Data Packet] ================" << std::endl;

    Packet dataPkt;

    std::string ip;
    int port;

    while (true) {

        std::vector<uint8_t> buffer;

        bool isReceived = socket.receiveFrom(buffer, ip, port);
        bool isDropped = shouldDrop(PACKET_DROP);

        dataPkt = Packet::deserialize(buffer);

        bool isDuplicate = (dataPkt.seqNum < seqNum);

        if (!isDropped && !isDuplicate && isReceived) {

            printLog(dataPkt, "RECEIVE DATA");
            break;

        } else {
            if (isDropped) {
                printError(dataPkt, "DROPPED DATA");
            } else if (isDuplicate) {
                printError(dataPkt, "DUPLICATE DATA");
                sendAckPacket(seqNum, ip, port);
            } else if (!isReceived) {
                std::cerr << "[ERROR] Failed to receive data packet." << std::endl;
            } else {
                std::cerr << "[ERROR] What?" << std::endl;
            }
        }
        
    }

    senderIP = ip;
    senderPort = port;

    return dataPkt;

}

void Receiver::sendAckPacket(int ackNum, std::string senderIP, int senderPort) {
    Packet ackPkt(ACK, ackNum);
    socket.sendTo(ackPkt.serialize(), senderIP, senderPort);

    printLog(ackPkt, "SEND ACK");
}

void Receiver::sendEOTAckPacket(int seqNum, std::string senderIP, int senderPort) {

    set_sigalrm_handler();

    Packet eotAckPkt = Packet(ACK, seqNum);

    std::cout << "[EOT Received] Wait of Transmission Ending." << std::endl;
    
    alarm(EOT_ACK_TIME);

    while (true) {
        
        socket.sendTo(eotAckPkt.serialize(), senderIP, senderPort);

    
        std::vector<uint8_t> buffer;
        std::string senderIP;
        int senderPort;
        bool isReceived = socket.receiveFrom(buffer, senderIP, senderPort);
        if (isReceived) {
            Packet pkt = Packet::deserialize(buffer);
            if (pkt.type == EOT) {
                continue;
            } else {
                printError(pkt, "Weird Packet");
            }
        } else {
            break;
        }
    }

    std::cout << "Transmission Ended." << std::endl;

}

void Receiver::receiveFile(const std::string& outputFile) {

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file!" << std::endl;
        return;
    }

    int expectedSeqNum = 0;

    while (true) {

        Packet dataPkt = receiveDataPacketRDT(expectedSeqNum, senderIP, senderPort);

        if (dataPkt.type == EOT) {
            sendEOTAckPacket(dataPkt.seqNum, senderIP, senderPort);
            break;
        }
        expectedSeqNum = dataPkt.seqNum + 1;

        sendAckPacket(dataPkt.seqNum, senderIP, senderPort);

        outFile.write(dataPkt.getData().data(), dataPkt.length);
        std::cout << "[Write] seqNum=" << dataPkt.seqNum << std::endl;

    }

    outFile.close();
}

