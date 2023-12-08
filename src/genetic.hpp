#ifndef GENETIC_H
#define GENETIC_H

#include "../structures/DataReader.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

void genetic(const vector<Node>& nodes, const vector<Request>& requests, const double &vehicleCapacity);
void removeMember(vector<vector<int>> &population, const vector<int> &to_remove);

double fitness(const vector<int> &solution, const vector<Request> &requests, const double &vehicleCapacity, const vector<vector<double>> &distanceMatrix);
double calculateCustomerDistance(const vector<int> &current_route, const vector<vector<double>> &distanceMatrix); // TODO: candidate for util

vector<vector<int>> initPopulation(const vector<Node> &customers, const size_t &populationSize);
vector<vector<int>> getRoutes(const vector<int> &solution, const vector<Request> &requests, const double &vehicleCapacity);
vector<int> mutation(const vector<int> &solution, const vector<Request> &requests, const double &vehicleCapacity);
vector<int> binaryTournament(const vector<vector<int>> &population, const vector<Request> &requests, const double &vehicleCapacity, const vector<vector<double>> &distanceMatrix);

pair<vector<int>, vector<int>> orderedCrossover(const vector<int> &parent1, const vector<int> &parent2);
pair<pair<double,double>, pair<vector<int>, vector<int>>> populationFitness(const vector<vector<int>> &population, const vector<Request> &requests, const double &vehicleCapacity, const vector<vector<double>> &distanceMatrix);

#endif
