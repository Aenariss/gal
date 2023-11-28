#ifndef REQ_H
#define REQ_H

#include "Node.hpp"

using namespace std;

// Class representing each node in the "world"
class Request {
    public:
        Request(int id, Node whereto, double quantity);
        int id;
        Node whereto;
        double quantity;
};

#endif
