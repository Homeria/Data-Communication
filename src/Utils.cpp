// src/Utils.cpp
#include "Utils.h"
#include <random>
#include <iostream>

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
    std::cout << "[" << type << "] : " << pkt.toString() << std::endl;
}

void printError(const Packet& pkt, const std::string& type) {
    std::cerr << "[" << type << "] : " << pkt.toString() << std::endl;
}