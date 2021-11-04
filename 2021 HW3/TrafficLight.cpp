// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "TrafficLight.hpp"

#include <iostream>

#include "constants.hpp"

extern int verbose;

TrafficLight::TrafficLight(const int myCNN, const std::string& myStreet) : currentColor(Color::red), myStreet(myStreet), myCNN(myCNN), isSynchronized(false) {}

void TrafficLight::setColor(const Color& newColor) {
    currentColor = newColor;
    if(verbose & (V_INTERSECTIONS | V_STREETS))
        std::cout << "Intersection " << myCNN << ", setting street " << myStreet << " to " << cname(newColor) << std::endl;
}

void TrafficLight::reset() {
    currentColor = Color::red;
}

std::string TrafficLight::cname(const Color& somecolor) {
    switch(somecolor) {
        case Color::red:
            return "red";
        case Color::green:
            return "green";
        case Color::yellow:
            return "yellow";
    }
    return "unknown";
}
