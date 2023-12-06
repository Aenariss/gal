#ifndef SAVINGS_HPP
#define SAVINGS_HPP

#include "../structures/DataReader.hpp"
#include <iostream>
#include <set>
#include <list>
#include <cmath>

using namespace std;

void savingsAlgorithm(const vector<Node>& nodes, const vector<Request>& requests, double vehicleCapacity);

class Savings {
public:
    int customerOneId;
    int customerTwoId;
    double value;

    Savings();

    /**
     * Calculates the savings based on distances between clients and depot as well as the distance between clients.
     * The saving means the distance saved by combined distribution rather than separated distribution for
     * more than one customer.
     * @param customerOneId id of the node representing the first customer
     * @param customerTwoId id of the node representing the second customer
     * @param distanceMatrix the precalculated matrix of distances between customers,
     *                       as well as their distance from the depot
     */
    Savings(int customerOneId, int customerTwoId, vector<vector<double>>& distanceMatrix);


    /**
     * Prints the representation of Savings to the standard output
     */
    void printOut() const;

    bool operator>(const Savings& other) const;
    bool operator==(const Savings& other) const;
};

struct SavingsRanker {
    bool operator()(const Savings &lhs, const Savings &rhs) const {
        return lhs.value > rhs.value;
    }
};

Savings getMaxSavings(multiset<Savings, SavingsRanker>& savings, int customerId, const vector<bool>& isServed);

class Route {
private:
    // The Savings algorithm needs to add IDs at the begining as well as the end of the route therefore list is used.
    list<int> route;            // Ids of customers on the route (excluding the start in the depot and end in the depot)
public:
    double vehicleCapacity;     // vehicle that will be covering the route
    double currentQuantity;     // quantity required by the customers on the route
    double distance;            // distance covered by the route

    /**
     * Constructs empty route with allocated capacity of the vehicle that will be covering the route.
     * @param vehicleCapacity capacity of the vehicle that will be covering the route
     */
    explicit Route(double vehicleCapacity);

    /**
     * Adds the given customer ID to the route. It allows to specify if the customer should be added at the beginning or
     * the end of the route. The currentQuantity and distance is updated accordingly to given parameters.
     * @param customerId  ID of the customer that will be added to the route
     * @param requestedQuantity  quantity of goods requested by the customer
     * @param start if true, the customer should be added at the start of the route. If false, the customer
     *              will be added at the end of the route
     * @param distanceMatrix matrix that contains distances between all nodes in the graph (customers as well as the depot)
     * @return
     */
    bool addCustomerIfCapacity(int customerId, double requestedQuantity, bool start, const vector<vector<double>>& distanceMatrix);

    /**
     * Appends given customer IDs to the end of the route. It also updates the currentQuantity required on the route as
     * well as the distance covered by the route. It is intended to initialize the route with pair of nodes that
     * have largest savings (as part of the Savings algorithm).
     * @param customerOneId ID of the first customer that will be added to the route
     * @param customerTwoId ID of the second customer that will be added to the route
     * @param requests requests containing the information about requested quantity of goods by a customer used to
     *                 update the currentQuantity parameter
     * @param distanceMatrix matrix that contains distances between all nodes in the graph (customers as well as the depot)
     * @return
     */
    bool appendCustomersIfCapacity(int customerOneId, int customerTwoId, const vector<Request>& requests, const vector<vector<double>>& distanceMatrix);

    /**
     * Adds the distance from depot to the beginning of route as well as the distance from the end of route to the depot.
     * Should be added when the route is complete (no additional customers will be added).
     * @param distanceMatrix matrix that contains distances between all nodes in the graph (customers as well as the depot)
     */
    void addDistancesToDepot(const vector<vector<double>>& distanceMatrix);

    /**
     * Retrieves the first customer id in the route.
     * @return Id of the first customer in the route
     */
    int getStart() const;

    /**
     * Retrieves the last customer ID in the route.
     * @return ID of the first customer in the route
     */
    int getEnd() const;

    /**
     * Retrieves the size of route.
     * @return
     */
    int getSize() const;

    /**
     * Prints the representation of the route to the standard output.
     */
    void printOut() const;

};

void createRouteForNotServedCustomers(const vector<bool>& isServed, vector<Route>& routes, double vehicleCapacity,
                                      const vector<Request>& requests, const vector<vector<double>>& distanceMatrix);

#endif //SAVINGS_HPP