// src/Utils.cpp
#include "Utils.h"
#include <random>

/**
 * 
 * @return true or false
 */
bool shouldDrop(double probability) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < probability;
}
