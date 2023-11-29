#ifndef REQ_H
#define REQ_H

#include "Node.hpp"

using namespace std;

// Class representing each request
class Request {
    public:
        Request(int id, Node whereto, double quantity);
        int id; // request id
        Node whereto; // Node of the request creator
        double quantity; // how much he requires
};

#endif
