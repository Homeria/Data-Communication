#include "Sender.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0]
        << " <sender_port> <receiver_ip> <receiver_port> <timeout> <file_name> <ack_drop_prob>\n";
        return 1;
    }

    int senderPort = std::stoi(argv[1]);
    std::string receiverIP = argv[2];
    int receiverPort = std::stoi(argv[3]);
    int timeout = std::stoi(argv[4]);
    std::string fileName = argv[5];
    double ackDropProb = std::stod(argv[6]);

    Sender sender;
    sender.run(fileName, receiverIP, receiverPort, ackDropProb, timeout);
    return 0;
}