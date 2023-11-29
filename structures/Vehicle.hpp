#ifndef VEHICLE_H
#define VEHICLE_H

#include "Node.hpp"

// Class representing each vehicle
class Vehicle {
    public:
        Vehicle(int type, Node departure, Node arrival, double capacity);
        int type; // vehicle type (maybe if there were more types?)
        Node departure; // Node from where the vehicle departs (the hub)
        Node arrival; // Node where the vehicle should return (the hub again)
        double capacity; // How much the vehicle can carry
};

#endif
