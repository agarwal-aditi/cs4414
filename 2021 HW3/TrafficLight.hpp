// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#pragma once

#include <string>

class TrafficLight {
public:
    enum Color : int { red = 0,
                       green = 1,
                       yellow = 2 };
    TrafficLight::Color currentColor;
    std::string myStreet;
    int myCNN;
    bool isSynchronized;
    static std::string cname(const Color&);
    TrafficLight(const int, const std::string&);
    void setColor(const Color&);
    void reset();
};
