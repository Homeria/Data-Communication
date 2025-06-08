#include "Receiver.h"

#define TIMEOUT_TIME 2
#define EOT_ACK_TIME 5

double PACKET_DROP = 0.01;

/**
 * * Signal handler for SIGALRM.
 * * This function is called when the alarm signal is received.
 * * It can be used to handle timeouts in the receiver.
 * * @param sig The signal number (not used here).
 */
void sigalrm_handler(int sig) {
    // std::cerr << "[TIMEOUT] Alarming!" << std::endl;
}

/**
 * * Set the signal handler for SIGALRM.
 * * This function sets up the signal handler to handle timeouts.
 * * It uses sigaction to register the handler.
 */
void set_sigalrm_handler() {
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // SA_RESTART를 끄는 것이 중요!
    sigaction(SIGALRM, &sa, NULL);
}


/**
 * * Run the receiver to listen for incoming packets and save the received file.
 * * @param outputFile The name of the output file where the received data will be saved.
 * * @param listenPort The port number on which the receiver will listen for incoming packets.
 * * @param dataDropProb The probability of dropping a data packet (default is 0.0, meaning no drop).
 */
void Receiver::run(const std::string& outputFile, int listenPort, double dataDropProb) {

    PACKET_DROP = dataDropProb;
    initLogFiles(false);
    
    if (!bindAndWait(listenPort)) {
        return;
    } else {
        std::cout << "instruct and bind socket successfully." << std::endl;
    }
    std::cout << "[END] 0. Set and Bind socket ===========================\n" << std::endl;
    std::cout << "[Test] wait the file" << std::endl;
    receiveFile(outputFile);

}

/**
 * * Bind the socket to the specified port and wait for incoming packets.
 * * @param listenPort The port number to bind the socket to.
 * * @return true if binding is successful, false otherwise.
 */
bool Receiver::bindAndWait(int listenPort) {
    if (!socket.bindSocket(listenPort)) {
        std::cerr << "Failed to bind on port " << listenPort << std::endl;
        return false;
    }
    return true;
}

/**
 * * Test function to receive a packet and print its contents.
 * * This function is used for debugging purposes.
 * * @return true if the test is successful, false otherwise.
 */
bool Receiver::test() {
    std::vector<uint8_t> buffer;
    socket.receiveFrom(buffer, senderIP, senderPort);
    Packet pkt = Packet::deserialize(buffer);
    std::cout << pkt.toString() << std::endl;
    
    return true;
}

/**
 * * Receive a data packet using the RDT protocol.
 * * This function handles packet loss, duplication, and retransmission.
 * * @param seqNum The expected sequence number of the packet.
 * * @param senderIP The IP address of the sender.
 * * @param senderPort The port number of the sender.
 * * @return The received Packet object.
 */
Packet Receiver::receiveDataPacketRDT(int seqNum, std::string& senderIP, int& senderPort) {
    
    std::cout << "\n[Receive Data Packet] =================================" << std::endl;

    // Sender -> Receiver : Data Packet
    Packet dataPkt;

    // while loop to handle retransmission and packet loss

    while (true) {

        std::vector<uint8_t> buffer;

        bool isReceived = socket.receiveFrom(buffer, senderIP, senderPort);
        dataPkt = Packet::deserialize(buffer);

        bool isDropped = shouldDrop(PACKET_DROP);
        bool isDuplicate = (dataPkt.seqNum < seqNum);

        // Not dropped, not duplicate, and received
        if (!isDropped && !isDuplicate && isReceived) {

            printLog(dataPkt, "RECEIVE DATA");
            writeLog(dataPkt, "RECEIVE DATA", false);
            break;

        } else {
            // Handle dropped, duplicate, or not received packets
            if (isDropped) {
                printError(dataPkt, "DROPPED DATA");
                writeLog(dataPkt, "DROPPED DATA", false);
            } else if (isDuplicate) {
                printError(dataPkt, "DUPLICATE DATA");
                writeLog(dataPkt, "DUPLICATE DATA", false);
                sendAckPacket(seqNum, senderIP, senderPort);
            } else if (!isReceived) {
                std::cerr << "[ERROR] Failed to receive data packet." << std::endl;
            } else {
                std::cerr << "[ERROR] What?" << std::endl;
            }
        }
    }
    return dataPkt;
}

/**
 * * Send an acknowledgment packet for the received data packet.
 * * This function sends an ACK packet to the sender with the acknowledgment number.
 * * @param ackNum The acknowledgment number to be sent.
 * * @param senderIP The IP address of the sender.
 * * @param senderPort The port number of the sender.
 */
void Receiver::sendAckPacket(int ackNum, std::string senderIP, int senderPort) {
    Packet ackPkt(ACK, ackNum);
    socket.sendTo(ackPkt.serialize(), senderIP, senderPort);
    printLog(ackPkt, "SEND ACK");
    writeLog(ackPkt, "SEND ACK", false);
}

/**
 * * Send an acknowledgment packet for the end of transmission (EOT).
 * * This function sends an EOT ACK packet to the sender and waits for further EOT packets.
 * * @param seqNum The sequence number of the EOT packet.
 * * @param senderIP The IP address of the sender.
 * * @param senderPort The port number of the sender.
 */
void Receiver::sendEOTAckPacket(int seqNum, std::string senderIP, int senderPort) {

    set_sigalrm_handler();

    Packet eotAckPkt = Packet(ACK, seqNum);

    std::cout << "[EOT Received] Wait of Transmission Ending." << std::endl;
    
    alarm(EOT_ACK_TIME);

    while (true) {
        
        socket.sendTo(eotAckPkt.serialize(), senderIP, senderPort);
        printLog(eotAckPkt, "SEND EOT ACK");
        writeLog(eotAckPkt, "SEND EOT ACK", false);
    
        std::vector<uint8_t> buffer;
        std::string senderIP;
        int senderPort;
        bool isReceived = socket.receiveFrom(buffer, senderIP, senderPort);

        bool isDropped = shouldDrop(PACKET_DROP);

        if(isDropped) {
            printError(eotAckPkt, "DROPPED EOT");
            writeLog(eotAckPkt, "DROPPED EOT", false);
            continue;
        }

        if (isReceived) {
            Packet pkt = Packet::deserialize(buffer);
            if (pkt.type == EOT) {
                printLog(pkt, "RECEIVE EOT");
                writeLog(pkt, "RECEIVE EOT", false);
                continue;
            } else {
                printError(pkt, "Weird Packet");
                writeLog(pkt, "RECEIVE WEIRD PACKET", false);
            }
        } else {
            break;
        }
    }

    std::cout << "Transmission Ended." << std::endl;

}

/**
 * * Receive a file from the sender and save it to the specified output file.
 * * This function handles the reception of data packets, acknowledgment packets, and end of transmission (EOT).
 * * @param outputFile The name of the output file where the received data will be saved.
 */
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

