#include <iostream>
#include <iostream>
#include <fstream>

#include "DataReader.hpp"
#include "../libs/pugixml.hpp"

using namespace std;

VRPDataReader::VRPDataReader(string filename) {
    // c++ doesnt have std library for xmls, we need to use a custom one
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    vector<Node> nodes;
    vector<Vehicle> vehicles;
    vector<Request> requests;

    // load info about nodes
    for (pugi::xml_node node : doc.child("instance").child("network").child("nodes").children("node")) {
        auto id = node.attribute("id").as_int();
        auto type = node.attribute("type").as_int();
        auto cx = stod(node.child("cx").child_value());
        auto cy = stod(node.child("cy").child_value());

        Node my_node = Node(id, type, cx, cy);
        nodes.push_back(my_node);
    }

    // load info about vehicles
    for (pugi::xml_node node : doc.child("instance").child("fleet").children("vehicle_profile")) {
        auto type = node.attribute("type").as_int();
        auto capacity = stod(node.child("capacity").child_value());

        auto dep_node_id = stoi(node.child("departure_node").child_value());
        auto arr_node_id = stoi(node.child("arrival_node").child_value());

        Node departure;
        Node arrival;
        
        for (auto node : nodes) {
            if (node.id == dep_node_id) {
                departure = node;
            }
            if (node.id == arr_node_id) {
                arrival = node;
            }
        }

        Vehicle my_vehicle = Vehicle(type, departure, arrival, capacity);
        vehicles.push_back(my_vehicle);
    }

    // load requests
    for (pugi::xml_node node : doc.child("instance").child("requests").children("request")) {
        auto id = node.attribute("id").as_int();
        auto whereto_id = node.attribute("node").as_int();
        auto quantity = stod(node.child("quantity").child_value());

        Node whereto;
        
        for (auto node : nodes) {
            if (node.id == whereto_id) {
                whereto = node;
            }
        }

        Request req = Request(id, whereto, quantity);
        requests.push_back(req);
    }

    this->nodes = nodes;
    this->vehicles = vehicles;
    this->requests = requests;
}
