#include "Packet.h"
#include <iomanip>
#include <cstring>

#define MAX_DATA_SIZE 50

// Constructor
Packet::Packet(int type , int seqNum, const std::string& data)
    : type(type), seqNum(seqNum), ackNum(0), length(data.size()), data(data)
{}

Packet::Packet(int type, int num)
    : type(type), seqNum(0), ackNum(0), length(0), data("")
{
    if (type == ACK) {
        ackNum = num;
    } else if (type == EOT) {
        seqNum = num;
    }
}

std::vector<uint8_t> Packet::serialize() const {
    std::vector<uint8_t> buffer;

    // type
    int t = htonl(type);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&t), reinterpret_cast<char*>(&t) + sizeof(int));

    // seqNum
    int s = htonl(seqNum);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&s), reinterpret_cast<char*>(&s) + sizeof(int));

    // ackNum
    int a = htonl(ackNum);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&a), reinterpret_cast<char*>(&a) + sizeof(int));

    // length
    int l = htonl(length);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&l), reinterpret_cast<char*>(&l) + sizeof(int));

    // data
    buffer.insert(buffer.end(), data.begin(), data.end());

    return buffer;
}

Packet Packet::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < 16) {
        throw std::runtime_error("Buffer too small to deserialize");
    }

    int net_type, net_seq, net_ack, net_len;

    std::memcpy(&net_type, &buffer[0], sizeof(int));
    std::memcpy(&net_seq,  &buffer[4], sizeof(int));
    std::memcpy(&net_ack,  &buffer[8], sizeof(int));
    std::memcpy(&net_len,  &buffer[12], sizeof(int));

    Packet pkt;
    pkt.type   = ntohl(net_type);
    pkt.seqNum = ntohl(net_seq);
    pkt.ackNum = ntohl(net_ack);
    pkt.length = ntohl(net_len);

    if (pkt.length > 0 && buffer.size() >= 16 + pkt.length) {
        pkt.data = std::string(buffer.begin() + 16, buffer.begin() + 16 + pkt.length);
    } else if (pkt.length > 0) {
        throw std::runtime_error("Data length mismatch in deserialization");
    }

    return pkt;

}


std::string Packet::toString() const {
    std::ostringstream oss;
    oss << "Packet { "
        << "type: " << type << ", "
        << "seqNum: " << seqNum << ", "
        << "ackNum: " << ackNum << ", "
        << "length: " << length << ", "
        << "data: \"" << data << "\" }";
    return oss.str();
}

std::string Packet::getData() const {
    return this->data;
}