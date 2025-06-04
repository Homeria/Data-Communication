#include "Sender.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file_path> <receiver_port>\n";
        return 1;
    }

    std::string filePath = argv[1];
    int port = std::stoi(argv[2]);
    std::string receiverIP = "127.0.0.1";

    Sender sender;
    sender.run(filePath, receiverIP, port);
    return 0;
}