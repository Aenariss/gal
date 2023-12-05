#ifndef GENETIC_H
#define GENETIC_H

#include "../structures/DataReader.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

void genetic(const VRPDataReader& reader);

double fitness(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix);

vector<vector<int>> initPopulation(vector<Node> customers, size_t populationSize);
vector<int> binaryTournament(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix);
vector<vector<int>> getRoutes(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix);
vector<int> mutation(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix);
double calculateCustomerDistance(vector<int> current_route, vector<vector<double>> distanceMatrix); // TODO: candidate for util
vector<int> binaryTournament(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix);
vector<vector<int>> removeMember(vector<vector<int>> population, vector<int> to_remove);

pair<vector<int>, vector<int>> orderedCrossover(vector<int> parent1, vector<int> parent2);
pair<pair<double,double>, pair<vector<int>, vector<int>>> populationFitness(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix);

#endif
