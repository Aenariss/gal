/**
 * An Improved Savings Method for Vehicle Routing Problem
 * This implementation is based on the following paper: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=7784340
 * Author: Peter Močáry <xmocar00>
**/

#include <algorithm>
#include <iomanip>
#include "savings.hpp"
#include "util.hpp"


void savingsAlgorithm(const vector<Node>& nodes, const vector<Request>& requests, double vehicleCapacity) {
    vector<Route> routes;
    multiset<Savings, SavingsRanker> savings;
    vector<bool> customersServed(nodes.size() - 1, false); // served status (excluding the depot)

    // Step one:
    // Calculate the distance between every two customers and between each customer to the depot
    vector<vector<double>> distanceMatrix = calculateDistanceMatrix(nodes);

    // Step two:
    // Calculate all savings between every two customers. Rank the savings and omit those below zero.
    for (int i = 0; i < (int)nodes.size()-1; i++) {
        for (int j = i + 1; j < (int)nodes.size()-1; j++) {
            // Note: customer IDs always start at 1
            Savings newSavings(i+1, j+1, distanceMatrix);
            if (newSavings.value < .0) {
                continue;
            }
            savings.insert(newSavings);
        }
    }

    do {
        // Step three:
        // Choose two customers with maximum savings satisfied the truck load limit as the initial route.
        Route route(vehicleCapacity);
        for (auto& candidateSavings: savings) {
            if (customersServed[candidateSavings.customerOneId-1] or customersServed[candidateSavings.customerTwoId-1]){
                continue;
            }
            bool addedToRoute = route.appendCustomersIfCapacity(candidateSavings.customerOneId, candidateSavings.customerTwoId,
                                                                requests, distanceMatrix);
            if (addedToRoute) {
                customersServed[candidateSavings.customerOneId-1] = true;
                customersServed[candidateSavings.customerTwoId-1] = true;
                break;
            }
        }


        vector<Savings> toDelete;
        for (auto& candidateNextSavings: savings) {
            if (customersServed[candidateNextSavings.customerOneId-1] and customersServed[candidateNextSavings.customerTwoId-1]) {
                continue;
            }
            // Step four:
            // 4.1) In the remainder of savings, choose the largest one that contains same customer of either the
            // start or end of previous route.
            // 4.2) Decide whether this savings is the largest one for this customer. If not, turn to the second
            // largest savings. Add the customer to previous route if the sum of the demand does not exceed
            // the truck load.

            // 4.1) first found is the largest thanks to sorted savings
            int candidateCustomerId = -1;
            bool start = candidateNextSavings.customerOneId == route.getStart() or candidateNextSavings.customerTwoId == route.getStart();
            if (candidateNextSavings.customerOneId == route.getStart() or candidateNextSavings.customerOneId == route.getEnd()) {
                candidateCustomerId = candidateNextSavings.customerTwoId;
            } else if (candidateNextSavings.customerTwoId == route.getStart() or candidateNextSavings.customerTwoId == route.getEnd()) {
                candidateCustomerId = candidateNextSavings.customerOneId;
            }

            if (candidateCustomerId != -1) { // A candidate that can be added to the start or end of route was found
                // 4.2)
                Savings maxSavings = getMaxSavings(savings, candidateCustomerId, customersServed);
                if (candidateNextSavings == maxSavings) {
                    bool added = route.addCustomerIfCapacity(candidateCustomerId,
                                                             requests[candidateCustomerId - 2].quantity, start,
                                                             distanceMatrix);
                    if (added) {
                        customersServed[candidateCustomerId-1] = true;
                    }
                }
            }
            // Step five:
            // Repeat step four till no customer can be added to the route subjected to the limit of the truck load
        }
        route.addDistancesToDepot(distanceMatrix);
        routes.push_back(route);

    // Step six:
    // Repeat steps three, four and step five till all customers have been added to the routes.
    } while (!all_of(customersServed.begin(), customersServed.end(), [](bool v) { return v; })); //TODO: condition

    double totalDistance = 0;
    int routeNum = 1;
    for (auto& route : routes) {
        cout << "#" << routeNum;
        route.printOut();
        totalDistance += route.distance;
        totalDistance += 1; // Penalty for care used
        routeNum++;
    }
    cout << totalDistance << endl;
}


Savings getMaxSavings(multiset<Savings, SavingsRanker>& savings, int customerId, const vector<bool>& isServed) {
    Savings maxSavings;
    for (auto& currentSavings : savings) {
        if (isServed[currentSavings.customerOneId-1] and isServed[currentSavings.customerTwoId-1]) {
            continue;
        }
        if (currentSavings.customerOneId == customerId or currentSavings.customerTwoId == customerId) {
            return currentSavings; // the first found is the max savings
        }
    }
    return maxSavings;
}

Savings::Savings(int customerOneId, int customerTwoId, vector<vector<double>>& distanceMatrix) {
    this->customerOneId = customerOneId;
    this->customerTwoId = customerTwoId;

    double distanceBetweenClients = distanceMatrix[customerOneId][customerTwoId];
    double depotDistanceClientOne = distanceMatrix[0][customerOneId];
    double depotDistanceClientTwo = distanceMatrix[0][customerTwoId];
    this->value = depotDistanceClientOne + depotDistanceClientTwo - distanceBetweenClients;
}

Savings::Savings() {
    customerTwoId = 0; // depot
    customerOneId = 0; // depot
    value = -1.0;      // invalid savings
}

bool Savings::operator>(const Savings &other) const {
    return this->value > other.value;
}


bool Savings::operator==(const Savings &other) const {
    return (this->customerOneId == other.customerOneId and
            this->customerTwoId == other.customerTwoId and
            this->value - other.value <= pow(10, 3));
}


void Savings::printOut() const {
    cout << setw(10) << value << " ";
    cout << "C" << customerOneId << " " << "C" << customerTwoId << endl;
}


bool Route::addCustomerIfCapacity(int customerId, double requestedQuantity, bool start, const vector<vector<double>>& distanceMatrix) {
    if (requestedQuantity + currentQuantity <= vehicleCapacity) {
        currentQuantity += requestedQuantity;
        if (start) {
            distance += distanceMatrix[customerId][this->getStart()];
            route.push_front(customerId);
        } else {
            distance += distanceMatrix[customerId][this->getEnd()];
            route.push_back(customerId);
        }
        return true;
    }
    return false;
}

bool Route::appendCustomersIfCapacity(int customerOneId, int customerTwoId, const vector<Request>& requests, const vector<vector<double>>& distanceMatrix) {
    double requestedQuantity = requests[customerOneId-2].quantity + requests[customerTwoId-2].quantity;

    if (requestedQuantity + currentQuantity <= vehicleCapacity) {
        currentQuantity += requestedQuantity;
        route.push_back(customerOneId);
        route.push_back(customerTwoId);
        distance += distanceMatrix[customerOneId][customerTwoId];
        return true;
    }
    return false;
}


Route::Route(double vehicleCapacity) {
    this->vehicleCapacity = vehicleCapacity;
    this->currentQuantity = 0;
    this->distance = 0;
}

int Route::getStart() const {
    return this->route.front();
}

int Route::getEnd() const {
    return this->route.back();
}

void Route::printOut() const {
    cout << " " << this->distance << " (" << this->currentQuantity << "/" << this->vehicleCapacity << ") ";
    for (auto& id : route) {
        cout << " " << id;
    }
    cout << endl;
}

void Route::addDistancesToDepot(const vector<vector<double>> &distanceMatrix) {
    this->distance += distanceMatrix[0][this->getStart()] + distanceMatrix[0][this->getEnd()];
}