/**
 * An Improved Savings Method for Vehicle Routing Problem
 * This implementation is based on the following paper: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=7784340
 * Author: Peter Močáry <xmocar00>
**/


#include <algorithm>
#include <chrono>
#include <iomanip>
#include "savings.hpp"
#include "util.hpp"

void savingsAlgorithm(const vector<Node>& nodes, const vector<Request>& requests, double vehicleCapacity) {
    // TIMESTAMP: Record time before the algorithm starts
    auto algorithmStart = chrono::high_resolution_clock::now();

    vector<Route> routes;
    int maxRoutesCnt = ceil((int) nodes.size()/2);
    multiset<Savings, SavingsRanker> savings;
    vector<bool> customersServed(nodes.size() - 1, false); // served status (excluding the depot)
    
    // Complexity - m (number of edges) and n (number of nodes)
    // m = 1/2 * n(n-1) ~= n^2

    // Space Complexity - 
    // O(m+n+n)
    // O(n^2+n+n)
    // O(n^2) - quadratic
    
    // Time Complexity 
    // O(m + m*log(n) + (m*n + m*(n+m) + n)*n)
    // O(n^2 + n^2*log(n) + (n^3 + n^2*(n+n^2) + n)*n)
    // O(n^2 + n^2*log(n) + (n^3 + n^3 + n^4 + n)*n)
    // O(n^2 + n^2*log(n) + n^4 + n^4 + n^5 + n^2)
    // O(n^5) - polynomial

    // Step one:
    // Calculate the distance between every two customers and between each customer to the depot
    // O(m)
    vector<vector<double>> distanceMatrix = calculateDistanceMatrix(nodes);

    // Step two:
    // Calculate all savings between every two customers. Rank the savings and omit those below zero.
    // time - O(m)
    for (int i = 2; i < (int)nodes.size()-1; i++) {
        for (int j = i + 1; j < (int)nodes.size()-1; j++) {
            // Note: customer IDs always start at 2
            Savings newSavings(i, j, distanceMatrix);
            if (newSavings.value < .0) {
                continue;
            }
            // O(log n)
            savings.insert(newSavings);
        }
    }
    int routesCnt = 0;
    //O(n/2) = O(n)
    do {
        // Step three:
        // Choose two customers with maximum savings satisfied the truck load limit as the initial route.
        Route route(vehicleCapacity);
        routesCnt++;
        // time - O(m)
        for (auto& candidateSavings: savings) {
            if (customersServed[candidateSavings.customerOneId-2] or customersServed[candidateSavings.customerTwoId-2]){
                continue;
            }
            // O(n)
            bool addedToRoute = route.appendCustomersIfCapacity(candidateSavings.customerOneId, candidateSavings.customerTwoId,
                                                                requests, distanceMatrix);
            if (addedToRoute) {
                customersServed[candidateSavings.customerOneId-2] = true;
                customersServed[candidateSavings.customerTwoId-2] = true;
                break;
            }
        }

        vector<Savings> toDelete;
        // O(m)
        for (auto& candidateNextSavings: savings) {
            if (customersServed[candidateNextSavings.customerOneId-2] and customersServed[candidateNextSavings.customerTwoId-2]) {
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

            if (candidateCustomerId != -1 and !customersServed[candidateCustomerId-2]) { // A candidate that can be added to the start or end of route was found
                // 4.2)
                // time - O(m)
                // NOTE: if the step of finding max savings is omitted entirely the algorithm seems to be giving better
                // results. A single implementation of this algorithm was found on github and it also omits this step.
                // However, the step was preserved to compare stick with definition of algorithm from the paper.
                Savings maxSavings = getMaxSavings(savings, candidateCustomerId, customersServed);
                if (candidateNextSavings == maxSavings) {
                    // time - O(n)
                    bool added = route.addCustomerIfCapacity(candidateCustomerId,
                                                             requests[candidateCustomerId-2].quantity, start,
                                                             distanceMatrix);
                    if (added) {
                        customersServed[candidateCustomerId-2] = true;
                    }
                }
            }
            // Step five:
            // Repeat step four till no customer can be added to the route subjected to the limit of the truck load
        }

        // NOTE: this is artificial fix for cases when savings don't contain pairs of remaining customers
        // The algorithm was not specified well enough to decide what happens in this case therefore we
        // assume that customers are served one by one.
        if (route.getSize() == 0) {
            createRouteForNotServedCustomers(customersServed, routes, vehicleCapacity,
                                             requests, distanceMatrix);
            break;
        }
        route.addDistancesToDepot(distanceMatrix);
        //O(n)
        routes.push_back(route);

    // Step six:
    // Repeat steps three, four and step five till all customers have been added to the routes.
    // O(n) - check if all customers were served
    } while (!all_of(customersServed.begin(), customersServed.end(), [](bool v) { return v; }) and routesCnt != maxRoutesCnt);

    // TIMESTAMP: record the time after the algorithm ends and calculate its duration
    auto algorithmEnd = chrono::high_resolution_clock::now();
    auto algorithmDuration = chrono::duration_cast<chrono::microseconds>(algorithmEnd - algorithmStart);

    double totalDistance = 0;
    int routeNum = 0;
    int routesWithOneNode = 0;
    int routesWithTwoNodes= 0;
    double unusedCapacity = .0;
    for (auto& route : routes) {
        routeNum++;
        totalDistance += route.distance;
        if (route.getSize() == 1) {
            routesWithOneNode++;
        } else if (route.getSize() == 2) {
            routesWithTwoNodes++;
        }
        unusedCapacity = route.vehicleCapacity - route.currentQuantity;
        cout << "#" << routeNum;
        route.printOut();
    }
    cout << "Overall distances " << totalDistance << endl;
    cout << "Vehicles " << routeNum << endl;
    cout << "Average number of customers " << nodes.size() / routeNum << endl;
    cout << "Number of routes linking only one customer " << routesWithOneNode<< endl;
    cout << "Number of routes linking only two customers " << routesWithTwoNodes << endl;
    cout << "Unused capacity " << unusedCapacity << endl;
    cout << "Time of the algorithm " << algorithmDuration.count() << " microseconds" << endl;
}

void createRouteForNotServedCustomers(const vector<bool>& isServed, vector<Route>& routes, double vehicleCapacity,
                                      const vector<Request>& requests, const vector<vector<double>>& distanceMatrix) {
    vector<int> idsNotInRouteYet;
    for (int i = 0; i < (int)isServed.size(); i++) {
        if (!isServed[i]) idsNotInRouteYet.push_back(i+2);
    }
    // create routes for every customer that was not yet served
    for (auto& id : idsNotInRouteYet) {
        Route newRoute(vehicleCapacity);
        newRoute.addCustomerIfCapacity(id, requests[id-2].quantity, false, distanceMatrix);
        routes.push_back(newRoute);
    }
}

Savings getMaxSavings(multiset<Savings, SavingsRanker>& savings, int customerId, const vector<bool>& isServed) {
    Savings maxSavings;
    for (auto& currentSavings : savings) {
        if (isServed[currentSavings.customerOneId-2] and isServed[currentSavings.customerTwoId-2]) {
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

    double distanceBetweenClients = distanceMatrix[customerOneId-1][customerTwoId-1];
    double depotDistanceClientOne = distanceMatrix[0][customerOneId-1];
    double depotDistanceClientTwo = distanceMatrix[0][customerTwoId-1];
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
            distance += distanceMatrix[customerId-1][this->getStart()-1];
            route.push_front(customerId);
        } else {
            distance += distanceMatrix[customerId-1][this->getEnd()-1];
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
        distance += distanceMatrix[customerOneId-1][customerTwoId-1];
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
    for (auto& id : route) {
        cout << " " << id;
    }
    cout << endl;
}

int Route::getSize() const {
    return this->route.size();
}

void Route::addDistancesToDepot(const vector<vector<double>> &distanceMatrix) {
    this->distance += distanceMatrix[0][this->getStart()-1] + distanceMatrix[0][this->getEnd()-1];
}
