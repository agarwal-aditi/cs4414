// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.

#pragma once

#include <memory>

class AlertEvent {
public:
    int when;
    static void runOne();
    static void runAll();
    static void resetQueue();
    void scheduleMe(const std::shared_ptr<AlertEvent>&, const int);
    virtual void event(const std::shared_ptr<AlertEvent>&, const int) = 0;
};
