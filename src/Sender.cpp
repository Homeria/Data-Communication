#include "Sender.h"

#define EOT_RETRANSMISSION 5


double PACKET_DROP = 0.01;
double TIMEOUT_TIME = 2.0;

/**
 * * Signal handler for SIGALRM.
 * * This function is called when the alarm signal is received.
 * * It can be used to handle timeouts in the sender.
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
 * * Run the sender to send a file to the specified destination.
 * * @param filePath The path of the file to be sent.
 * * @param destIP The IP address of the destination.
 * * @param destPort The port number of the destination.
 */
void Sender::run(const std::string& filePath, const std::string& destIP, int destPort, double ackDropProb, int timeout) {

    PACKET_DROP = ackDropProb;
    TIMEOUT_TIME = timeout;

    initLogFiles(true);
    
    sendFile(filePath, destIP, destPort);

}

/**
 * * Test function to send a simple packet to the destination.
 * * @param destIP The IP address of the destination.
 * * @param destPort The port number of the destination.
 * * @return true if the packet is sent successfully, false otherwise.
 */
bool Sender::test(const std::string& destIP, int destPort) {

    Packet pkt(DATA, 0, "Test! ABCD!");
    std::cout << pkt.toString() << std::endl;
    socket.sendTo(pkt.serialize(), destIP, destPort);

    return true;
}

/**
 * * Send a data packet using Reliable Data Transfer (RDT) protocol.
 * * This function handles sending the packet, waiting for an acknowledgment, and retransmitting if necessary.
 * * @param pkt The Packet object to be sent.
 * * @param destIP The IP address of the destination.
 * * @param destPort The port number of the destination.
 * * @return true if the packet is sent successfully, false otherwise.
 */
bool Sender::sendDataPacketRDT(Packet pkt, const std::string& destIP, int destPort) {

    std::cout << "\n[Send Data Packet] " << "================" << std::endl;


    set_sigalrm_handler();

    while (true) {

        socket.sendTo(pkt.serialize(), destIP, destPort);

        printLog(pkt, "SEND DATA");
        writeLog(pkt, "SEND DATA", true);

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
                writeLog(pkt, "DROPPED ACK", true);
                ackReceived = socket.receiveFrom(ackBuffer, senderIP, senderPort);
            } else {
                alarm(0);
                printLog(ackPkt, "RECEIVE ACK");
                writeLog(ackPkt, "RECEIVE ACK", true);
                break;
                
            }
        } else {
            printError(pkt, "TIMEOUT DATA");
            writeLog(pkt, "TIMEOUT DATA", true);
            alarm(0);
            continue;
        }

    }

    return true;
}


/**
 * * Send a file to the specified destination using the RDT protocol.
 * * This function reads the file in chunks, sends data packets, and handles end-of-transmission (EOT).
 * * @param filePath The path of the file to be sent.
 * * @param destIP The IP address of the destination.
 * * @param destPort The port number of the destination.
 * * @note The file is read in chunks of 50 bytes, and the EOT packet is sent after all data packets.
 * *       The function will retry sending the EOT packet up to EOT_RETRANSMISSION times.
 * *       If the file cannot be opened, an error message is printed.
 * *       If the packet cannot be sent, an error message is printed.
 * *       The function uses the sendDataPacketRDT function to handle sending data packets reliably.
 */
void Sender::sendFile(const std::string& filePath, const std::string& destIP, int destPort) {

    // if opening file is failed
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        printError(Packet(), "File open failed");
        return;
    }

    char buffer[MAX_DATA_SIZE];
    int seqNum = 0;

    // 파일을 끝까지 읽을 때까지 반복

    while (inFile.read(buffer, sizeof(buffer)) || inFile.gcount() > 0) {

        std::streamsize bytesRead = inFile.gcount();
        std::cout << "bytesRead: " << bytesRead << std::endl;
        if (bytesRead > 1) {

            Packet pkt(DATA, seqNum, std::string(buffer, bytesRead));
            bool isSent = sendDataPacketRDT(pkt, destIP, destPort);

            if (isSent) {
                seqNum++;
            } else {
                printError(pkt, "The Packet not sent");
                continue;
            }
        }
    }

    int count = 0;
    while (count < EOT_RETRANSMISSION) {

        count++;

        Packet pktEOT(EOT, seqNum);

        bool isSent = sendDataPacketRDT(pktEOT, destIP, destPort);
        if (isSent) {
            printLog(pktEOT, "SEND EOT");
            writeLog(pktEOT, "SEND EOT", true);
            std::cout << "Sending File and \"Finish\" is succeed." << std::endl;
            break;
        } else {
            continue;
        }
    }
    
    inFile.close();

}