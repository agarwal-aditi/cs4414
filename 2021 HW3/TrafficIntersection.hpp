// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#pragma once

#include <map>
#include <memory>

#include "AlertEvent.hpp"
#include "csv.hpp"
#include "TrafficLight.hpp"

class TrafficIntersection : public AlertEvent {
    int numberOfStreets;
    int activeStreet;
    int myGreenTime;
    bool isSync;
    csvRow myRow;

public:
    bool isInitialized;
    std::string pos;
    std::shared_ptr<std::vector<std::shared_ptr<TrafficLight>>> myLights;
    TrafficIntersection(const csvRow&, std::shared_ptr<std::vector<std::shared_ptr<TrafficLight>>>&);
    virtual void event(const std::shared_ptr<AlertEvent>&, const int);
    static void reset();
    void setTimer(const std::shared_ptr<AlertEvent>&, const int, const int, std::string, bool isSync);
    static TrafficIntersection* getIntersection(const int);
};

extern std::map<int, std::shared_ptr<AlertEvent>> intersections;
