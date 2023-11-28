#ifndef VEHICLE_H
#define VEHICLE_H

#include "Node.hpp"

// Class representing each vehicle
class Vehicle {
    public:
        Vehicle(int type, Node departure, Node arrival, double capacity);
        int type;
        Node departure;
        Node arrival;
        double capacity;
};

#endif
