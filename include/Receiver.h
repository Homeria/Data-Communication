#define RECEIVER_H

#include <string>
#include "UDPSocket.h"

class Receiver {
public:
    void run(const std::string& outputFile, int listenPort);

private:
    UDPSocket socket;
    std::string senderIP;
    int senderPort;

    bool test();
    bool bindAndWait(int listenPort);
    void receiveGreeting();
    std::string receiveFileName();
    void sendOK();
    void receiveFile(const std::string& outputFile);
    void sendWellDone();
};