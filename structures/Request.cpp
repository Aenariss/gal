#include "Request.hpp"

Request::Request(int id, Node whereto, double quantity) {
    this->id = id;
    this->whereto = whereto;
    this->quantity = quantity;
}
