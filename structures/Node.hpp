#ifndef NODE_H
#define NODE_H

#include <string>

using namespace std;

// Class representing each node in the "world"
class Node {
    public:
        Node(int id, int type, double x, double y);
        Node();
        int id; // Node id
        int type; // Node type (hub/client)
        double x; // X coord
        double y; // Y coord
};

#endif
