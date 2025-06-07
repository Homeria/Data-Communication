// include/Utils.h
#ifndef UTILS_H

#define UTILS_H



#include <random>
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "Packet.h"
bool shouldDrop(double probability);
void printLog(const Packet& pkt, const std::string& type);
void printError(const Packet& pkt, const std::string& type);
void writeLog(Packet pkt, const std::string& eventType, bool isSender);
void initLogFiles(bool isSender);
std::string getCurrentTimeString();

#endif
