// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#pragma once

#include <map>
#include <memory>
#include <string>

#include "AlertEvent.hpp"
#include "Street.hpp"

class Car : public AlertEvent {
    int car_n;
    int traffic;
    std::shared_ptr<Street> currentStreet;
    int current_row;
    int start_time;
    bool isActive;
    bool isStuck;
    void destReached(const int);
    int driving_time(const int, const int);

public:
    static int64_t total_driving_time;
    static int active_cars;
    Car(const int, const int, const int);
    static void reset();
    virtual void event(const std::shared_ptr<AlertEvent>&, const int);
    void setTimer(const std::shared_ptr<AlertEvent>&, const int);
};
