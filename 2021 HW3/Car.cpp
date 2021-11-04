// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#include "Car.hpp"

#include <cmath>
#include <iostream>

#include "constants.hpp"
#include "csv.hpp"
#include "TrafficIntersection.hpp"

extern int verbose;
extern csv TL, SC;

int Car::active_cars = 0;
int stuckCars = 0;
int64_t Car::total_driving_time = 0;
std::vector<std::shared_ptr<AlertEvent>> cars;

double compute_distance(const std::pair<double, double>& point1, const std::pair<double, double>& point2) {
    const std::pair<double, double> DEGREE_TO_MILES(54.74, 68.97);
    const double long_miles = (point2.first - point1.first) * DEGREE_TO_MILES.first;
    const double lat_miles = (point2.second - point1.second) * DEGREE_TO_MILES.second;
    return std::sqrt(long_miles * long_miles + lat_miles * lat_miles);
}

double compute_time(const double& distance, const uint32_t& speed) {
    return std::ceil((distance * 3600) / speed);
}
const double one_degree_long_miles = 54.74;
const double one_degree_lat_miles = 69.97;

int Car::driving_time(const int cnn1, const int cnn2) {
    size_t idx1, idx2;
    std::string long1 = TrafficIntersection::getIntersection(cnn1)->pos;
    std::string long2 = TrafficIntersection::getIntersection(cnn2)->pos;
    long1 = long1.substr(long1.find('-') + 1);
    long2 = long2.substr(long2.find('-') + 1);
    double cnn1_long = -stod(long1, &idx1), cnn2_long = -stod(long2, &idx2);
    double cnn1_lat = stod(long1.substr(idx1)), cnn2_lat = stod(long2.substr(idx2));
    std::pair<double, double> point1(cnn1_long, cnn1_lat);
    std::pair<double, double> point2(cnn2_long, cnn2_lat);
    double dist = compute_distance(point1, point2);
    double time = compute_time(dist, traffic == LIGHT ? 30.0 : (traffic == MEDIUM ? 10 : 3.0));
    if(verbose & V_CARS)
        std::cout << (traffic == LIGHT ? "(light traffic)" : (traffic == MEDIUM ? "(medium traffic)" : "(heavy traffic)")) << "... after roundoff, driving from CNN " << cnn1 << " to " << cnn2 << " (distance = " << dist << "miles) takes " << time << "secs" << std::endl;
    return time;
}

Car::Car(const int car_n, const int start_time, const int traffic) : car_n(car_n), traffic(traffic), currentStreet(nullptr), current_row(SKIPROWS), start_time(start_time), isActive(true), isStuck(false) {
    ++active_cars;
}

void Car::setTimer(const std::shared_ptr<AlertEvent>& myHandle, const int when) {
    if(verbose & V_CARS)
        std::cout << "schedule car " << car_n + 1 << ":" << start_time << " at time=" << when << std::endl;
    scheduleMe(myHandle, when);
}

void Car::reset() {
    total_driving_time = 0;
    stuckCars = 0;
}

void Car::event(const std::shared_ptr<AlertEvent>& myHandle, const int now) {
    int currentCNN = stoi(SC[current_row][car_n]);
    auto ti = TrafficIntersection::getIntersection(currentCNN);
    if(isStuck) {
        isStuck = false;
        --stuckCars;
    }
    if(verbose & (V_CARS | V_ALERTS))
        std::cout << "Car " << car_n + 1 << ":" << start_time << " event at time " << now << "(current row is " << current_row << ")" << std::endl;
    bool fnd = false;
    if(currentStreet != nullptr) {
        for(uint streetnum = 0; streetnum < ti->myLights->size(); streetnum++)
            if((*ti->myLights)[streetnum]->myStreet == currentStreet->myName) {
                if((*ti->myLights)[streetnum]->currentColor == TrafficLight::Color::red) {
                    if(verbose & V_CARS)
                        std::cout << "... red light on " << (*ti->myLights)[streetnum]->myStreet << std::endl;
                    scheduleMe(myHandle, now + 1);
                    return;
                }
                if(verbose & V_CARS)
                    std::cout << "... green or yellow on " << (*ti->myLights)[streetnum]->myStreet << std::endl;
                fnd = true;
                break;
            }
    }
    if(!fnd) {
        if((*ti->myLights)[0]->currentColor == TrafficLight::Color::red) {
            if(verbose & V_CARS)
                std::cout << "... red light on " << (*ti->myLights)[0]->myStreet << std::endl;
            scheduleMe(myHandle, now + 1);
            return;
        }
        if(verbose & V_CARS)
            std::cout << "... green or yellow on " << (*ti->myLights)[0]->myStreet << std::endl;
        fnd = true;
    }
    int nextCNN = stoi(SC[current_row + 1][car_n]);
    if(nextCNN == 0) {
        if(verbose & V_CARS)
            std::cout << "... destination has been reached! " << std::endl;
        destReached(now);
        return;
    }
    auto st = streets[((int64_t)currentCNN << 32) | (int64_t)nextCNN];
    if(st == nullptr) {
        std::cerr << "ERROR: Null street pointer from " << currentCNN << " to " << nextCNN << std::endl;
        abort();
    }
    if(verbose & V_CARS) {
        std::cout << "currentCNN = " << currentCNN << ", nextCNN = " << nextCNN << std::endl;
        if(currentStreet == nullptr)
            std::cout << "No current street (car is just entering the simulation)... focusing on street " << st->myName << " from " << currentCNN << " to " << nextCNN << std::endl;
        else
            std::cout << "Current street is " << currentStreet->myName << ", next focusing on street " << st->myName << " from " << currentCNN << " to " << nextCNN << std::endl;
    }
    if(verbose & V_STREETS)
        std::cout << "Current capacity of << " << st->myName << " permits " << st->additionalCapacity << " more cars before it becomes backed up" << std::endl;
    if(st->additionalCapacity == 0) {
        if(verbose & V_CARS) {
            int car_count = 0;
            std::cout << "... traffic backed up on " << st->myName << ": {";
            for(auto ca : cars) {
                auto car = static_cast<Car*>(ca.get());
                if(car->isActive && car->currentStreet == st) {
                    std::cout << "car " << car->car_n << ":" << car->start_time << " ";
                    car_count++;
                }
            }
            std::cout << "} " << st->myName << ", recheck in 1 sec" << std::endl;
            if(car_count > st->initialCapacity)
                std::cout << "*** ERROR: Street " << st->myName << " should have " << car_count << " cars on it! " << std::endl;
        }
        if(++stuckCars == active_cars) {
            std::cerr << "Gridlock detected at time " << now << ", aborting." << std::endl;
            abort();
        }
        isStuck = true;
        scheduleMe(myHandle, now + 1);
        return;
    }
    if(verbose & V_CARS)
        std::cout << "... advancing onto " << st->myName << std::endl;
    if(currentStreet != nullptr)
        currentStreet->additionalCapacity++;
    currentStreet = st;
    st->additionalCapacity--;
    current_row++;
    // People drive faster on streets with synchronized lights
    if(st->isSynchronized)
        scheduleMe(myHandle, now + driving_time(st->fromCNN, st->toCNN) * 2 / 3);
    else
        scheduleMe(myHandle, now + driving_time(st->fromCNN, st->toCNN));
}

void Car::destReached(const int now) {
    if(currentStreet != nullptr)
        currentStreet->additionalCapacity++;
    total_driving_time += now - start_time;
    isActive = false;
    --active_cars;
    if(verbose & V_CARS) {
        if(active_cars > 0)
            std::cout << "Car " << car_n << ":" << start_time << " has arrived at its destination! (there are still " << active_cars << " active cars)" << std::endl;
        else
            std::cout << "Car " << car_n << ":" << start_time << " has arrived at its destination!  This run is finished!" << std::endl;
    }
}
