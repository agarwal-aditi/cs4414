// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "AlertEvent.hpp"

#include <iostream>
#include <queue>

#include "Car.hpp"
#include "constants.hpp"

extern int verbose;

class compareAE {
public:
    bool operator()(const std::shared_ptr<AlertEvent>& lhs, const std::shared_ptr<AlertEvent>& rhs) const {
        return lhs->when > rhs->when;
    }
};

std::priority_queue<std::shared_ptr<AlertEvent>, std::vector<std::shared_ptr<AlertEvent>>, compareAE> myPriorityQueue;

void AlertEvent::scheduleMe(const std::shared_ptr<AlertEvent>& ae, const int when) {
    ae->when = when;
    myPriorityQueue.emplace(ae);
}

void AlertEvent::runAll() {
    if(verbose & V_ALERTS)
        std::cout << "Alert runAll with a queue that initially has " << myPriorityQueue.size() << " elements" << std::endl;
    while(!myPriorityQueue.empty() && Car::active_cars > 0)
        AlertEvent::runOne();
}

void AlertEvent::runOne() {
    auto ev = myPriorityQueue.top();
    myPriorityQueue.pop();
    if(verbose & V_ALERTS)
        std::cout << "Alert callback, time = " << ev->when << ", queue now has " << myPriorityQueue.size() << " elements" << std::endl;
    ev->event(ev, ev->when);
}

void AlertEvent::resetQueue() {
    if(verbose & V_ALERTS)
        std::cout << "Empty alert priority queue" << std::endl;
    while(!myPriorityQueue.empty())
        myPriorityQueue.pop();
}
