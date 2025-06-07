#include "Utils.h"

/**
 * @brief Decide whether to drop a packet based on a given probability
 * @return true or false
 */
bool shouldDrop(double probability) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < probability;
}

void printLog(const Packet& pkt, const std::string& type) {
    std::cout << "[" << getCurrentTimeString() << ", " << type << "] : " << pkt.toString() << std::endl;
}

void printError(const Packet& pkt, const std::string& type) {
    std::cerr << "[" << getCurrentTimeString() << ", " << type << "] : " << pkt.toString() << std::endl;
}

/**
 * @brief Write a log entry for a packet event (send/receive) to the appropriate log file
 * @param pkt The Packet object to log
 * @param eventType The type of event (e.g., "SEND", "RECEIVE")
 * @param isSender True if the event is from the sender's perspective, false if from the receiver's
 */
void writeLog(Packet pkt, const std::string& eventType, bool isSender) {
    std::string logFilePath;
    if (isSender) {
        logFilePath = "log/sender.txt";
    } else {
        logFilePath = "log/receiver.txt";
    }
    std::ofstream logFile(logFilePath, std::ios::app);
    if (!logFile) return;

    logFile << "[" << getCurrentTimeString() << "] [" << eventType << "]\n"
        << "  seqNum: " << pkt.seqNum << ", ackNum: " << pkt.ackNum << ", length: " << pkt.length << "\n"
        << "  data: " << pkt.data << "\n\n";
}

void initLogFiles(bool isSender) {
    if (isSender) {
    std::ofstream senderLog("log/sender.txt", std::ios::trunc);
    } else {
        std::ofstream receiverLog("log/receiver.txt", std::ios::trunc);
    }
}

std::string getCurrentTimeString() {

    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S")
        << ":" << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}