#include "Receiver.h"
#include "Packet.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string.h>

#define PACKET_DROP 0.1
#define TIMEOUT_TIME 2



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

void Receiver::receiveFile(const std::string& outputFile) {

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file!" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer;
    int expectedSeqNum = 0;

    while (true) {


        bool receivedRawData = socket.receiveFrom(buffer, senderIP, senderPort);

        if (shouldDrop(PACKET_DROP)) {
            std::cerr << "[DROPPED] Data Packet is Dropped : " << std::endl;
            continue;
        }

        Packet dataPkt = Packet::deserialize(buffer);

        std::cout << "[Receive] " << dataPkt.toString() << std::endl;

        if (dataPkt.type == EOT) {
            std::cout << "[EOT Received] End of Transmission." << std::endl;
            break;

        } else {
            
            outFile.write(dataPkt.getData().data(), dataPkt.length);
            std::cout << "[Write] seqNum=" << dataPkt.seqNum << std::endl;

            Packet ackPkt(ACK, expectedSeqNum);
            socket.sendTo(ackPkt.serialize(), senderIP, senderPort);
            std::cout << "[Send ACK] " << ackPkt.toString() << std::endl;
        
        }
    }

    outFile.close();
    std::cout << "Receiving File completed successfully." << std::endl;
}