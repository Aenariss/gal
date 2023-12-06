
#include <cstring>
#include <filesystem>
#include "genetic.hpp"
#include "savings.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    string algo;
    string data;
    string usage = ("gal <option> <data-path>\n"
                    "\t<option>: One of the available options (with argument if needed)\n"
                    "\t  --help (-h) show this help message.\n"
                    "\t  --algorithm (-a) specified an algorithm as argument ['savings'|'genetic']\n"
                    "\t<data-path>: Path to the file with the representation of the CVRP problem.\n");
    if (argc < 2 or strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
        cout << usage << endl;
        exit(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "--algorithm") == 0 or strcmp(argv[1], "-a") == 0) {
        if (argc < 3 or (strcmp(argv[2], "savings") == 0 and strcmp(argv[2], "genetic") == 0)) {
            cerr << "--algorithm requires an argument ['savings'|'genetic']\n";
            exit(EXIT_FAILURE);
        }
        algo = argv[2];
        if (argc < 4) {
            cerr << "last argument should be a path to data file.\n";
            exit(EXIT_FAILURE);
        }
        data = argv[3];
    } else {
        cout << usage << endl;
        exit(EXIT_FAILURE);
    }

    // Load data
    VRPDataReader reader = VRPDataReader(data);

    vector<Node> nodes = reader.nodes;          // the 0th node is the depot
    vector<Request> requests = reader.requests; // each customer has single request
    vector<Vehicle> vehicles = reader.vehicles; // the number of vehicles is not taken into
                                                // account only the capacity which is taken from the first vehicle
    double vehicleCapacity = vehicles[0].capacity;
    if (algo == "savings") {
        savingsAlgorithm(nodes, requests, vehicleCapacity);
    } else if (algo == "genetic") {
        genetic(nodes, requests, vehicleCapacity);
    }
    return 0;
}
