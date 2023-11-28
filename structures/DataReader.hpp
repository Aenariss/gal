#ifndef READER_H
#define READER_H

#include <string>
#include <vector>
#include "Node.hpp"
#include "Vehicle.hpp"
#include "Request.hpp"

using namespace std;

class VRPDataReader {
    public:
        VRPDataReader(string filename);
        vector<Node> nodes;
        vector<Vehicle> vehicles;
        vector<Request> requests;
};

#endif
