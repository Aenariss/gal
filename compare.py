"""
    Simple script to compare results of both algorithms
"""

from os import listdir
import subprocess
from re import search

DATA_DIR = "./data/"
GENETIC_RUN = ["./gal", "--algorithm", "genetic"]
SAVINGS_RUN = ["./gal", "--algorithm", "savings"]

ALGOS = [GENETIC_RUN, SAVINGS_RUN]

files = [DATA_DIR + x for x in listdir(DATA_DIR) if x.endswith(".xml")]

routes = []
results = []

all_files = len(files)
i = 1
crashed = []

for file in files:
    print("Running the comparison for file", i, "/", all_files)
    i += 1
    routes_one_run = []
    results_one_run = []
    for algo in ALGOS:
        route = []
        overall = vehicles = avg = only_2 = unused = None
        try:
            result = subprocess.run(algo + [file], capture_output=True, text=True, timeout=5)
        except:
            result_values = (overall, vehicles, avg, only_2, unused)
            results_one_run.append(result_values)
            routes_one_run.append(route)
            crashed.append(file)
            continue
        
        lines_result = result.stdout.split("\n")
        for line in lines_result:
            if len(line) > 0:
                if line[0] == "#":
                    route.append(line)
                elif line[0] == "O": # Overall dist
                    overall = float(search("([0-9]*[.])?[0-9]+", line).group())
                elif line[0] == "V": # Vehicles
                    vehicles = float(search("(\d+)", line).group())
                elif line[0] == "A": # Average
                    avg = float(search("([0-9]*[.])?[0-9]+", line).group())
                elif line[0] == "N": # Only 2
                    only_2 = float(search("(\d+)", line).group())
                elif line[0] == "U": # Unused
                    unused = float(search("([0-9]*[.])?[0-9]+", line).group())

        result_values = (overall, vehicles, avg, only_2, unused)
        results_one_run.append(result_values)
        routes_one_run.append(route)

    routes.append((routes_one_run[0], routes_one_run[1]))
    results.append(results_one_run)

for result in results:
    genetic = result[0]
    savings = result[1]

    for (gen, sav) in zip(genetic, savings):
        print(gen, sav)


# maybe plot some routes and results later?
print(results)
