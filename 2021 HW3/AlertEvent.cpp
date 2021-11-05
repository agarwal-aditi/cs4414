// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "AlertEvent.hpp"

#include <iostream>
#include <queue>

#include "Car.hpp"
#include "constants.hpp"

extern int verbose;
int LENGTH = 20000;
extern int time_track;

class compareAE {
public:
    bool operator()(const std::shared_ptr<AlertEvent>& lhs, const std::shared_ptr<AlertEvent>& rhs) const {
        return lhs->when > rhs->when;
    }
};

std::priority_queue<std::shared_ptr<AlertEvent>, std::vector<std::shared_ptr<AlertEvent>>, compareAE> myPriorityQueue;

std::vector<std::vector<std::shared_ptr<AlertEvent>>> theVector(LENGTH);

void AlertEvent::scheduleMe(const std::shared_ptr<AlertEvent>& ae, const int when) {
    ae->when = when;
    int index;
    if(when>=LENGTH)
        index = when%LENGTH;
    else
        index = when;
    theVector[index].push_back(ae);
    // myPriorityQueue.emplace(ae);
}

void AlertEvent::runAll() {
    // if(verbose & V_ALERTS)
    //     std::cout << "Alert runAll with a queue that initially has " << myPriorityQueue.size() << " elements" << std::endl;
    // while(!myPriorityQueue.empty() && Car::active_cars > 0)
    while(Car::active_cars > 0)
        AlertEvent::runOne();
}

void AlertEvent::runOne() {
    int index;
    if(time_track>=LENGTH)
        index = time_track%LENGTH;
    else
        index = time_track;
    while(!theVector[index].empty()){
        auto ev = theVector[index].back();
        theVector[index].pop_back();
        if(verbose & V_ALERTS)
            std::cout << "Alert callback, time = " << ev->when << ", queue now has " << myPriorityQueue.size() << " elements" << std::endl;
        ev->event(ev, ev->when);
    }
    time_track++;

}

void AlertEvent::resetQueue() {
    time_track = 0;
    if(verbose & V_ALERTS)
        std::cout << "Empty alert priority queue" << std::endl;
    for(int i = 0; i< LENGTH; i++){
        theVector[i] =  std::vector<std::shared_ptr<AlertEvent>>();
    }
}
