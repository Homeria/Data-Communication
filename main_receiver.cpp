#include "Receiver.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <data_drop_prob>\n";
        return 1;
    }

    std::string output_file = "output.txt";
    int port = std::stoi(argv[1]);
    double dataDropProb = std::stod(argv[2]);
    Receiver receiver;
    receiver.run(output_file, port, dataDropProb);
    return 0;
}