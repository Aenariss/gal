/**
 * Genetic algorithm implementation for the GAL project
 * Author: Vojtech Fiala <xfiala61>
 * Partially inspired by https://github.com/krishna-praveen/Capacitated-Vehicle-Routing-Problem
**/

#include "genetic.hpp"

using namespace std;

/* Function to calculate the distance between 2 nodes */
double distance(Node first, Node second) {
    auto x_dist = first.x - second.x;
    auto y_dist = first.y - second.y;

    return sqrt(x_dist * x_dist + y_dist * y_dist);
}

void print2D(vector<vector<int>> vec) {
    for (auto line : vec) {
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

/**
 * Function to initialize the population randomly
*/
vector<vector<int>> initPopulation(vector<Node> customers, size_t populationSize) {

    vector<vector<int>> population;
    vector<int> customer_ids;
    size_t n_of_customers = customers.size();

    for (size_t i = 1; i < n_of_customers; i++) { // Start from 1 because the depot is not a customer (and it's always the first one)
        customer_ids.push_back(customers[i].id);
    }

    auto random_number_generator = default_random_engine();

    // Add new basic solutions until we reach the wanted population size. 
    // Note that populationSize shouldn't be higher than (number of customers)! (factorial)
    // If that wasn't the case, there would be duplicates
    while (population.size() < populationSize) {
        shuffle(begin(customer_ids), end(customer_ids), random_number_generator); // generate a permutation

        if(find(population.begin(), population.end(), customer_ids) == population.end()) { // If permutation isn't already present, append it
            population.push_back(customer_ids);
        }
    }

    return population;
}

/**
 * Function to create a distance matrix containing distances between nodes 
 * For the distance matrix we assume that the IDs of the customers are ordered from 1 to n
 * The reason for the matrix is that it's better to calculate the distances only once and not repeat it every time
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

/**
 * Function to calculate the distance in the given route using the preCalculated distanceMatrix
 * Implicitly adds the distance from the depot to the first customer and the distance from the last customer back to the depot
*/
double calculateCustomerDistance(vector<int> current_route, vector<vector<double>> distanceMatrix) {
    double total_dist = 0;

    // Add distance from depot to the first customer
    int customer_matrix_position = current_route[0]-1; // Customer 1 is on the position 0 because numbering starts from 1
    total_dist += distanceMatrix[0][customer_matrix_position];

    int prev_customer = customer_matrix_position;

    // Calculate distances between customers in the route
    for (auto customer : current_route) {
        customer_matrix_position = customer-1;
        total_dist += distanceMatrix[prev_customer][customer_matrix_position];
        prev_customer = customer_matrix_position;
    }

    total_dist += distanceMatrix[prev_customer][0]; // add the final customer's distance to the depot
    return total_dist;
}

/**
 * Function to calculate the fitness of the solution
 * Adds euclidean distances between each point and adds a penalty in the form of the amount of vehicles needed to fulfill the route (capacity constraint)
*/
double fitness(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix) {
    auto requests = reader.requests; // the requests are sorted by the ID of the customer
    double vehicle_capacity = reader.vehicles[0].capacity; // all vehicle share the same capacity
    double total_vehicle_capacity = vehicle_capacity;
    int vehicles_penalty = 0;
    double travelled_distance = 0;

    vector<int> current_route;

    for (auto customer_id : solution) { // go through each node representing a customer in the solution
        // find the requested amount of the goods
        auto request_id = customer_id - 2; // the requests always start with node 2 (because 1 is the depot) and therefore customer with ID 2 has request n. 0
        auto req = requests[request_id];
        if (req.whereto.id != customer_id) {
            cerr << "This should not happen, means there was a mismatch in the data file and therefore reuqest need to use a hashtable instead of a sorted vector" << endl;
            exit(1);
        }
        auto load = req.quantity;
        vehicle_capacity -= load;
        // if vehicle load goes under0, it means we need another vehicle and therefore apply a penalty and calculate total distance travelled by the now full vehicle
        if (vehicle_capacity < 0) {
            vehicles_penalty++; // apply penalty
            vehicle_capacity = total_vehicle_capacity - load; // substract the current load to another vehicle's capacity cuz this one can't fit it inside
            travelled_distance += calculateCustomerDistance(current_route, distanceMatrix);
            current_route = {}; current_route.push_back(customer_id);
        }
        else {
            current_route.push_back(customer_id);
        }
    }

    // If the current route is not empty, we need to finish it 
    if (current_route.size() > 0) {
        travelled_distance += calculateCustomerDistance(current_route, distanceMatrix);
        vehicles_penalty++;
    }
    auto total_fitness = travelled_distance + vehicles_penalty;
    return total_fitness;
}

/**
 * Function to get the routes from a solution
*/
vector<vector<int>> getRoutes(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix) {
    auto requests = reader.requests; // the requests are sorted by the ID of the customer
    double vehicle_capacity = reader.vehicles[0].capacity; // all vehicle share the same capacity
    double total_vehicle_capacity = vehicle_capacity;
    vector<vector<int>> routes;

    vector<int> current_route;

    for (auto customer_id : solution) { // go through each node representing a customer in the solution
        // find the requested amount of the goods
        auto request_id = customer_id - 2; // the requests always start with node 2 (because 1 is the depot) and therefore customer with ID 2 has request n. 0
        auto req = requests[request_id];
        if (req.whereto.id != customer_id) {
            cerr << "This should not happen, means there was a mismatch in the data file and therefore reuqest need to use a hashtable instead of a sorted vector" << endl;
            exit(1);
        }
        auto load = req.quantity;
        vehicle_capacity -= load;
        // if vehicle load goes under0, it means we need another vehicle and therefore apply a penalty and calculate total distance travelled by the now full vehicle
        if (vehicle_capacity < 0) {
            vehicle_capacity = total_vehicle_capacity - load; // substract the current load from another vehicle's capacity cuz this one can't fit it inside
            routes.push_back(current_route); // add the completed route to the routes list
            current_route = {}; current_route.push_back(customer_id); // add current customer to a new route
        }
        else {
            current_route.push_back(customer_id);
        }
    }

    // If the current route is not empty, we need to finish it 
    if (current_route.size() > 0) {
        routes.push_back(current_route);
    }
    
    return routes;
}

/**
 * Function to apply the mutation that randomly swaps 2 customers that are not on the same way
*/
vector<int> mutation(vector<int> solution, VRPDataReader reader, vector<vector<double>> distanceMatrix) {

    /*
    // Only mutate with a probability of 30%
    if (rand() % 100 >= 30) {
        return solution;
    }
    */

    vector<vector<int>> routes = getRoutes(solution, reader, distanceMatrix);

    int route_index1; int route_index2;
    // choose the routes from which we will randomly swap, they must not be the same
    while (true) {
        route_index1 = rand() % routes.size();
        route_index2 = rand() % routes.size();

        if (route_index1 != route_index2) {
            break;
        } 
    }

    // pick a random customer in each of those routes
    auto customer_index1 = rand() % routes[route_index1].size();
    auto customer_index2 = rand() % routes[route_index2].size();

    // swap the customers in the routes
    auto tmp = routes[route_index1][customer_index1];
    routes[route_index1][customer_index1] = routes[route_index2][customer_index2];
    routes[route_index2][customer_index2] = tmp;

    // flatten the route and return it as a new solution
    vector<int> mutant;
    for (auto route : routes) {
        for (auto customer : route) {
            mutant.push_back(customer);
        }
    }

    return mutant;
}

/**
 * Function to select an eligible parent from 2 random selections, where the better one is chosen
*/
vector<int> binaryTournament(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix) {
    auto pop_size = population.size();

    auto potentialParent1 = population[rand() % pop_size];
    auto potentialParent2 = population[rand() % pop_size];

    auto p1_score = fitness(potentialParent1, reader, distanceMatrix); // calculate how good the potential parent 1 is
    auto p2_score = fitness(potentialParent2, reader, distanceMatrix);

    return (p1_score > p2_score ? potentialParent1 : potentialParent2); // pick the better parent
}

/**
 * Function to apply the ordered crossover to generate new offspring
 * [1,2,5,6 | 7,8 | 9,10] parent1
 * [1,5,2,6 | 9,10 | 7,8] parent2
 * [1,5,2,6,  7,8  9,10] offspring1
 * [1,2,5,6,  9,10  7,8] offspring2
*/
pair<vector<int>, vector<int>> orderedCrossover(vector<int> parent1, vector<int> parent2) {
    vector<int> offspring1(parent1.size(), -1);
    vector<int> offspring2(parent1.size(), -1);

    auto lower_boundary = rand() % parent1.size();
    auto upper_boundary = rand() % parent1.size();

    // If lower > upper, swap them
    if (lower_boundary > upper_boundary) {
        auto tmp = lower_boundary;
        lower_boundary = upper_boundary;
        upper_boundary = tmp;
    }

    auto tmp_parent1 = parent1;
    auto tmp_parent2 = parent2;

    // Fill the new proto offspring with selected substrings and delete the selected substrings from the other parent
    for (auto i = lower_boundary; i <= upper_boundary; i++) {
        offspring1[i] = parent1[i];
        offspring2[i] = parent2[i];

        // Delete those elems that are already in the substring from the parents
        auto p2_remove = find(tmp_parent2.begin(), tmp_parent2.end(), offspring1[i]);
        auto p1_remove = find(tmp_parent1.begin(), tmp_parent1.end(), offspring2[i]);
        tmp_parent2.erase(p2_remove);
        tmp_parent1.erase(p1_remove);
    }
    // Now, tmp_parent1 & 2 contain only the numbers that arent present in the proto-child and those can be added to the offspring
    auto p1_ctr = 0;
    auto p2_ctr = 0;
    for (size_t i = 0; i < offspring1.size(); i++) {
        if (offspring1[i] == -1) {
            offspring1[i] = tmp_parent2[p2_ctr++];
            offspring2[i] = tmp_parent1[p1_ctr++];
        }
    }
    //print1D(parent1); print1D(parent2); print1D(offspring1); print1D(offspring2); cout << lower_boundary << " " << upper_boundary << endl;
    //exit(1);
    
    return make_pair(offspring1, offspring2);
}

/**
 * Function to calculate the fitness value of each member of the population and return the worst score & corresponding member
*/
pair<pair<double,double>, pair<vector<int>, vector<int>>> populationFitness(vector<vector<int>> population, VRPDataReader reader, vector<vector<double>> distanceMatrix) {
    double highest_score = 0;
    double lowest_score = __DBL_MAX__;
    vector<int> worst_member;
    vector<int> best_member;
    for (auto member : population) {
        auto score = fitness(member, reader, distanceMatrix);
        // If his result is worse (higher), mark him as the new worst
        if (score > highest_score) {
            highest_score = score;
            worst_member = member;
        }
        else if (score < lowest_score) {
            lowest_score = score;
            best_member = member;
        }
    }
    return make_pair(make_pair(highest_score, lowest_score), make_pair(worst_member, best_member));
}

/**
 * Function to remove given element from the vector
*/
vector<vector<int>> removeMember(vector<vector<int>> population, vector<int> to_remove) {
    auto index_to_remove = find(population.begin(), population.end(), to_remove);
    population.erase(index_to_remove);
    return population;
}

/**
 * Funcion to run the genetic algorithm
*/
void genetic() {

    // This all will be in a cycle going through all the data files which outputs the results into another folder
    VRPDataReader reader = VRPDataReader("./data/B-n31-k05.xml");

    size_t iteration_limit = 5000;
    auto customers = reader.nodes;
    auto distanceMatrix = calculateDistanceMatrix(customers);

    auto population = initPopulation(customers, 50); // 200 is the recommended amount

    // Running the algorithm
    for (size_t i = 0; i < iteration_limit; i++) {
        // Select parents using the binary tournament method
        vector<int> parent1 = binaryTournament(population, reader, distanceMatrix);
        vector<int> parent2 = binaryTournament(population, reader, distanceMatrix);

        // Create an offspring using ordered crossover
        pair<vector<int>, vector<int>> offsprings = orderedCrossover(parent1, parent2);
        auto offspring1 = offsprings.first;
        auto offspring2 = offsprings.second;

        // Mutate the offspring with a certain probability
        offspring1 = mutation(offspring1, reader, distanceMatrix);
        offspring2 = mutation(offspring2, reader, distanceMatrix);

        // Find the worst score in population and the correspnding weak member
        pair<pair<double, double>, pair<vector<int>, vector<int>>> worst_member = populationFitness(population, reader, distanceMatrix);
        auto worst_score = worst_member.first.first;
        auto worst_solution = worst_member.second.first;

        // Evaluate the offsprings against the lowest score in population
        auto offspring1_score = fitness(offspring1, reader, distanceMatrix);
        auto offspring2_score = fitness(offspring2, reader, distanceMatrix);

        double better_score = offspring1_score > offspring2_score ?  offspring2_score : offspring1_score;

        // Choose the better offspring to use as a replacement (Another approach would be to use both - the better to replace the worst, then recalculate and the other replace the next worst)
        vector<int> better_offspring = offspring1_score > offspring2_score ? offspring2 : offspring1;

        // Replace the worst member with offspring
        if (better_score < worst_score) {
            population = removeMember(population, worst_solution);
            population.push_back(offspring1);
        }
    }

    // Find the best score
    pair<pair<double, double>, pair<vector<int>, vector<int>>> best_member = populationFitness(population, reader, distanceMatrix);
    auto best_solution = best_member.second.second;
    cout << best_member.first.second << endl;

    // Find the best routes
    vector<vector<int>> routes = getRoutes(best_solution, reader, distanceMatrix);
    print2D(routes);
}
