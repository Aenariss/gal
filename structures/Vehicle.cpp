#include "Vehicle.hpp"

Vehicle::Vehicle(int type, Node departure, Node arrival, double capacity) {
    this->type = type;
    this->departure = departure;
    this->arrival = arrival;
    this->capacity = capacity;
}
