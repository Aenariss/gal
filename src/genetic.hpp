#ifndef GENETIC_H
#define GENETIC_H

#include "../structures/DataReader.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

void genetic();

double distance(Node first, Node second);
double fitness(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix);
double calculateCustomerDistance(vector<int> current_route, vector<vector<double>> distanceMatrix);

vector<vector<int>> initPopulation(vector<Node> customers, size_t populationSize);
vector<vector<double>> calculateDistanceMatrix(vector<Node> customers);
vector<int> binaryTournament(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix);

#endif
