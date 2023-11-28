#include "Node.hpp"

Node::Node(int id, int type, double x, double y) {
    this->id = id;
    this->type = type;
    this->x = x;
    this->y = y;
}

Node::Node() {
    this->id = 0;
    this->type = 0;
    this->x = 0.0;
    this->y = 0.0;
}
