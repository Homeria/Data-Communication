#include "TransportLayer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void TransportLayer::init_sender(const std::string& ip, int port) {
    // UDP 소켓 생성 및 주소 설정
}

void TransportLayer::send_message(const std::string& msg) {
    // sendto() 호출
}
