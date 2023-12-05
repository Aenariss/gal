//
// Created by ptr on 12/3/23.
//

#ifndef UTIL_HPP
#define UTIL_HPP

#include "../structures/DataReader.hpp"

using namespace std;

void print1D(vector<int> vec);
void print2D(vector<vector<int>> vec);

double distance(Node first, Node second);
vector<vector<double>> calculateDistanceMatrix(vector<Node> customers);

void printDistanceMatrix(vector<vector<double>>& distanceMatrix);

#endif //UTIL_HPP
