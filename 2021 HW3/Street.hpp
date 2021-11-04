// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#pragma once

#include <map>
#include <memory>
#include <string>

class Street {
public:
    int additionalCapacity;
    int initialCapacity;
    int fromCNN;
    int toCNN;
    bool isSynchronized;
    std::string myName;
    bool isDestination;
    Street(const int, const int, const int, const std::string&, const bool, const bool);
};

extern std::map<int64_t, std::shared_ptr<Street>> streets;
