// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "Street.hpp"

#include <iostream>

#include "constants.hpp"

std::map<int64_t, std::shared_ptr<Street>> streets;
extern int verbose;

Street::Street(const int additionalCapacity, const int fromCNN, const int toCNN, const std::string& myName, const bool isDestination, const bool isSynchronized) : additionalCapacity(additionalCapacity), initialCapacity(additionalCapacity), fromCNN(fromCNN), toCNN(toCNN), isSynchronized(isSynchronized), myName(myName), isDestination(isDestination) {
    if(verbose & V_STREETS)
        std::cout << "Instantiate street [" << myName << ", capacity = " << additionalCapacity << ", synchronized=" << isSynchronized << "]" << std::endl;
}
