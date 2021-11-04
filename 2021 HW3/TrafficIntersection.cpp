// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "TrafficIntersection.hpp"

#include <iostream>

#include "constants.hpp"

extern int verbose;
std::map<int, std::shared_ptr<AlertEvent>> intersections;
const int POS = 34;  // Column AI in TrafficSignals.csv

TrafficIntersection::TrafficIntersection(const csvRow& myRow, std::shared_ptr<std::vector<std::shared_ptr<TrafficLight>>>& myLights) : isSync(false), myRow(myRow), pos(myRow[POS]), myLights(myLights) {}

TrafficIntersection* TrafficIntersection::getIntersection(const int cnn) {
    return static_cast<TrafficIntersection*>(intersections[cnn].get());
}

void TrafficIntersection::event(const std::shared_ptr<AlertEvent>& myHandle, const int now) {
    if(verbose & V_INTERSECTIONS)
        std::cout << "Intersection[CNN=" << myRow[0] << "] event at time " << now << std::endl;
    auto tl = (*myLights.get())[activeStreet];
    switch(tl->currentColor) {
        case TrafficLight::Color::red:
            tl->setColor(TrafficLight::Color::green);
            if(isSync)
                scheduleMe(myHandle, tl->isSynchronized ? (myGreenTime + now) : (UNSYNCSTAYSGREEN + now));
            else
                scheduleMe(myHandle, myGreenTime + now);
            break;
        case TrafficLight::Color::green:
            tl->setColor(TrafficLight::Color::yellow);
            scheduleMe(myHandle, YELLOWDURATION + now);
            break;
        case TrafficLight::Color::yellow:
            tl->setColor(TrafficLight::red);
            activeStreet = (activeStreet + 1) % myLights->size();
            auto tl = (*myLights.get())[activeStreet];
            tl->setColor(TrafficLight::Color::green);
            scheduleMe(myHandle, myGreenTime + now);
            break;
    }
}

void TrafficIntersection::reset() {
    for(auto& tiae : intersections) {
        auto ti = static_cast<TrafficIntersection*>(tiae.second.get());
        ti->myGreenTime = 0;
        ti->activeStreet = 0;
        ti->isInitialized = false;
    }
}

void TrafficIntersection::setTimer(const std::shared_ptr<AlertEvent>& myHandle, int gt, const int turnGreenAt, const std::string streetName, bool iss) {
    // Figure out which traffic light this corresponds to
    int initialActiveStreet = 0;
    for(auto tl : *myLights)
        if(tl->myStreet == streetName)
            break;
        else
            ++initialActiveStreet;
    initialActiveStreet %= myLights->size();
    myGreenTime = gt;

    if(verbose & V_INTERSECTIONS)
        std::cout << "Intersection[CNN=" << myRow[0] << "] street " << streetName << " (traffic light " << initialActiveStreet << ") turns green at time " << turnGreenAt << ", with next event after delay " << myGreenTime << std::endl;

    int remainingGYTime;
    if(turnGreenAt > 0) {
        // In this case, some other light is initially green... let's figure it out!
        remainingGYTime = turnGreenAt;
        while(remainingGYTime > myGreenTime + YELLOWDURATION) {
            remainingGYTime -= myGreenTime + YELLOWDURATION;
            if(--initialActiveStreet < 0)
                initialActiveStreet += myLights->size();
        }
    } else
        remainingGYTime = myGreenTime + YELLOWDURATION;
    activeStreet = initialActiveStreet;
    isInitialized = true;
    isSync = iss;
    scheduleMe(myHandle, 0);
    if(verbose & V_INTERSECTIONS)
        std::cout << "Intial active light will be " << (*myLights.get())[activeStreet]->myStreet << " (traffic light " << activeStreet << "), for a duration of " << remainingGYTime << "s (yellow during final " << YELLOWDURATION << "s)" << std::endl;
}
