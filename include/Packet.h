#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <arpa/inet.h>

enum PacketType {
    DATA = 0,
    ACK = 1,
    EOT = 2
};

/**
 * @brief Packet
 * @param type 0: Data, 1: Ack, 2: EOT
 * @param seqNum Packet Sequence Number
 * @param data Payload(Data) in Packet
 */
class Packet {
public:
    // Header fields
    int type;       // 0: DATA, 1: ACK, 2: EOT
    int seqNum;
    int ackNum;
    int length;     // data length (0 ~ 1000)
    std::string data;

    Packet() : type(0), seqNum(0), ackNum(0), length(0), data("") {}

    // Data
    Packet(int type, int seqNum, const std::string& data);

    // ACK or EOT
    Packet(int type, int seqNum);

    std::string toString() const;

    std::vector<uint8_t> serialize() const;

    std::string getData() const;

    static Packet deserialize(const std::vector<uint8_t>& buffer);

    

};
