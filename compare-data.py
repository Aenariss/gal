import sys
import matplotlib.pyplot as plt

savings_results_file = sys.argv[1]
genetic_results_file = sys.argv[2]


class ResultData:
    def __init__(self, data):
        self.nodes_cnt: float = data[0]
        self.distance: float = data[1]
        self.vehicles: float = data[2]
        self.avg_customers_cnt: float = data[3]
        self.one_customer_in_route_cnt: float = data[4]
        self.two_customers_in_route_cnt: float = data[5]
        self.unused_capacity: float = data[6]
        self.time: float = data[7]
        self.space: float = data[8]


def parse_file(file: str):
    with open(file, 'r') as fd:
        results = []
        for line in fd:
            line = line.strip().split(',')
            single_run_data = []
            for value in line:
                single_run_data.append(float(value))
            single_run_results = ResultData(single_run_data)
            results.append(single_run_results)
        return results


def plot_time_complexity(results):
    nodes_cnt = []
    time = []
    for result in results:
        if result.nodes_cnt > 100:
            continue
        nodes_cnt.append(result.nodes_cnt)
        time.append(result.time)
    plt.scatter(x=nodes_cnt, y=time)
    plt.xlabel('Nodes in graph')
    plt.ylabel('Time in microseconds')
    plt.show()


def plot_space_complexity(results):
    nodes_cnt = []
    space = []
    for result in results:
        if result.nodes_cnt > 100:
            continue
        nodes_cnt.append(result.nodes_cnt)
        space.append(result.space)
    plt.scatter(x=nodes_cnt, y=space)
    plt.xlabel('Nodes in graph')
    plt.ylabel('Space in bytes')
    plt.show()


def calculate_avgs(results_savings, results_genetic):
    assert len(results_genetic) == len(results_savings)
    distance_ratios = []
    vehicle_ratios = []
    avg_customers_cnt_ratios = []
    one_customer_in_route_cnt_sum_s = 0
    one_customer_in_route_cnt_sum_g = 0
    two_customers_in_route_cnt_sum_s = 0
    two_customers_in_route_cnt_sum_g = 0
    unused_capacity_ratios = []
    skipped_capacity_cnt = 0
    runtime_ratios = []
    divisor = 0
    for result_s, result_g in zip(results_savings, results_genetic):
        if result_s.nodes_cnt < 100:
            continue
        divisor += 1
        distance_ratios.append(result_s.distance / result_g.distance)
        vehicle_ratios.append(result_s.vehicles / result_g.vehicles)
        avg_customers_cnt_ratios.append(result_s.avg_customers_cnt / result_g.avg_customers_cnt)
        one_customer_in_route_cnt_sum_s += result_s.one_customer_in_route_cnt
        one_customer_in_route_cnt_sum_g += result_g.one_customer_in_route_cnt
        two_customers_in_route_cnt_sum_s += result_s.two_customers_in_route_cnt
        two_customers_in_route_cnt_sum_g += result_g.two_customers_in_route_cnt
        runtime_ratios.append(result_s.time / result_g.time)
        if result_g.unused_capacity != 0:
            unused_capacity_ratios.append(result_s.unused_capacity / result_g.unused_capacity)
        else:
            skipped_capacity_cnt += 1

    # results_cnt = len(results_savings)
    print("Average ratio of savings / genetic for selected properties:")
    print(f"\tdistance: {sum(distance_ratios) / divisor}")
    print(f"\tvehicles used: {sum(vehicle_ratios) / divisor}")
    print(f"\taverage count of customers on a route: {sum(avg_customers_cnt_ratios) / divisor}")
    print(f"\tunused_capacity: {sum(unused_capacity_ratios) / (divisor - skipped_capacity_cnt)}")
    print(f"\trun-time: {sum(runtime_ratios) / divisor}")
    print("One customer in a route:")
    print(f"\tsavings: {one_customer_in_route_cnt_sum_s}")
    print(f"\tgenetic: {one_customer_in_route_cnt_sum_g}")
    print("Two customers in a route:")
    print(f"\tsavings: {two_customers_in_route_cnt_sum_s}")
    print(f"\tgenetic: {two_customers_in_route_cnt_sum_g}")


savings_results = parse_file(savings_results_file)
genetic_results = parse_file(genetic_results_file)

calculate_avgs(savings_results, genetic_results)
print("Plotting time complexity for Genetic:")
plot_time_complexity(genetic_results)
print("Plotting space complexity for Genetic:")
plot_space_complexity(genetic_results)
print("Plotting time complexity for Savings:")
plot_time_complexity(savings_results)
print("Plotting space complexity for Savings:")
plot_space_complexity(savings_results)

