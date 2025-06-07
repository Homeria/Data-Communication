// include/Utils.h
#ifndef UTILS_H
#define UTILS_H
#include <string>
#include "Packet.h"
bool shouldDrop(double probability);
void printLog(const Packet& pkt, const std::string& type);
void printError(const Packet& pkt, const std::string& type);

#endif
