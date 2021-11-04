// (c) Ken Birman.  This code is provided only for use in CS4144, fall 2021, homework 3
// The author specifically prohibits posting of this code on public sites such as
// Chegg, Course Hero, etc.
#pragma once

const int SKIPROWS = 2;
const int SYNCSTAYSGREEN = 60;
const int UNSYNCSTAYSGREEN = 30;
const int YELLOWDURATION = 10;

const int LIGHT = 0, MEDIUM = 1, HEAVY = 2;
const int UNSYNC = 0, SYNC = 1;

// Verbose flags
const int V_CARS = 0x01;
const int V_LIGHTS = 0x02;
const int V_INTERSECTIONS = 0x04;
const int V_STREETS = 0x08;
const int V_PARSE = 0x10;
const int V_ALERTS = 0x20;
const int V_SYNC = 0x40;
