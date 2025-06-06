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

int send_count = 1;
int receive_count = 1;


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

            std::cout << "[Receive] " << dataPkt.toString() << std::endl;
            break;

        } else {
            if (isDropped) {
                std::cerr << "[DROPPED] " << dataPkt.toString() << std::endl;
            } else if (isDuplicate) {
                std::cerr << "[DUPLICATE] " << dataPkt.toString() << std::endl;
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
    std::cout << "[Send ACK] " << ackPkt.toString() << std::endl;
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
            std::cout << "[EOT Received] End of Transmission." << std::endl;
            break;


        }
        expectedSeqNum = dataPkt.seqNum;

        outFile.write(dataPkt.getData().data(), dataPkt.length);
        std::cout << "[Write] seqNum=" << dataPkt.seqNum << std::endl;

        sendAckPacket(dataPkt.seqNum, senderIP, senderPort);
    }

    outFile.close();
    std::cout << "Receiving File completed successfully." << std::endl;
}