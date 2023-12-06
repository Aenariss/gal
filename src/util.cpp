#include "util.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>


void print2D(vector<vector<int>> vec) {
    auto lineCounter = 1;
    for (auto line : vec) {
        cout << "#" << lineCounter++ << " ";
        for (auto column : line) {
            cout << column << " ";
        }
        cout << endl;
    }
}

void print1D(vector<int> vec) {
    for (auto column : vec) {
        cout << column << " ";
    }
    cout << endl;
}


/* Function to calculate the distance between 2 nodes */
double distance(Node first, Node second) {
    auto x_dist = first.x - second.x;
    auto y_dist = first.y - second.y;

    return sqrt(x_dist * x_dist + y_dist * y_dist);
}

/**
 * Function to create a distance matrix containing distances between nodes
 * For the distance matrix we assume that the IDs of the customers are ordered from 1 to n
 * The reason for the matrix is that it's better to calculate the distances only once and not repeat it every time
 * TODO: could be optimized since it is symmetrical
 **/
vector<vector<double>> calculateDistanceMatrix(vector<Node> customers) {

    size_t n_of_customers = customers.size();

    vector<vector<double>> distanceMatrix(n_of_customers, vector<double> (n_of_customers, 0));

    for (auto customer : customers) {
        int matrix_pos_x = customer.id - 1; // Node IDs start with 1 (the depot), need to lower this to start indexing from 0

        // For each customer, go through the customer list and calculate distances
        for (auto next_customer : customers) {
            int matrix_pos_y = next_customer.id - 1;
            distanceMatrix[matrix_pos_x][matrix_pos_y] = distance(customer, next_customer);
        }
    }
    return distanceMatrix;
}

void printDistanceMatrix(vector<vector<double>> &distanceMatrix) {
    // print header
    cout << setw(10) << " ";
    cout << setw(10) << "D0";
    for (int i = 1; i < (int)distanceMatrix.size(); i++) {
        cout << setw(9) << "C" << i;
    }
    cout << endl;

    // print distances with side header
    for (int i = 0; i < (int)distanceMatrix.size(); i++) {
        // Print side header
        if (i == 0) {
            cout << setw(10) << "D0";
        } else {
            cout << setw(10) << "C" << i;
        }
        // Print distances
        for (auto& distance : distanceMatrix[i]) {
            cout << setw(10) << distance;
        }
        cout << endl;
    }
}
