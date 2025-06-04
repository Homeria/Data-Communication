#include "Receiver.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <output_file> <port>\n";
        return 1;
    }

    std::string output_file = argv[1];
    int port = std::stoi(argv[2]);

    Receiver receiver;
    receiver.run(output_file, port);
    return 0;
}