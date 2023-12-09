"""
    Simple script to compare results of both algorithms
"""

from os import listdir, path
import matplotlib.pyplot as plt
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

def comparisons():
    results = []
    i = 1
    GENERAL_RESULTS_FILE = "./results/general.txt"
    if (not path.exists(GENERAL_RESULTS_FILE)):

        f = open(GENERAL_RESULTS_FILE, "w")
        for file in files:

            print("Running the comparison for file", i, "/", all_files)
            i += 1
            routes_one_run = []
            results_one_run = []
            for algo in ALGOS:
                route = []
                overall = vehicles = avg = only_2 = unused = time = None
                try:
                    result = subprocess.run(algo + [file], capture_output=True, text=True, timeout=5)
                except:
                    result_values = (overall, vehicles, avg, only_2, unused, time)
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
                        elif line[0] == "T": # Time
                            time = float(search("([0-9]*[.])?[0-9]+", line).group())

                result_values = (overall, vehicles, avg, only_2, unused, time)

                results_one_run.append(result_values)
                routes_one_run.append(route)

            routes.append((routes_one_run[0], routes_one_run[1]))
            results.append(results_one_run)
            k = 0
            for result_one in results_one_run:
                if (k):
                    a = "S"
                else:
                    a = "G"
                k += 1
                f.write(a + "," + str(result_one[0]) + ',' + str(result_one[1]) + ',' + str(result_one[2]) + ',' + str(result_one[3]) + ',' + str(result_one[4]) + ',' + str(result_one[5]) + '\n')
        f.close()
    else:
        f = open(GENERAL_RESULTS_FILE, "r")
        k = 0
        tmp = []
        results = []
        for line in f.readlines():
            line = line.split(',')
            if (k%2):
                tmp.append((line[0], float(line[1]), float(line[2]), float(line[3]), float(line[4]), float(line[5]), float(line[6])))
                results.append(tmp)
            else:
                tmp.append((line[0], float(line[1]), float(line[2]), float(line[3]), float(line[4]), float(line[5]), float(line[6])))
            k += 1
    shorter = []
    vehicles_comp = []
    avgCust = []
    only_two = []
    unused_total = []
    times_t = []
    for result in results:
        genetic = result[0]
        savings = result[1]
        vehiclesG = genetic[2];vehiclesS = savings[2]
        distanceG = genetic[1];distanceS = savings[1]
        avgCustG = genetic[3]; avgCustS = savings[3]
        only_2G = genetic[4]; only_2S = savings[4]
        unused_G = genetic[5]; unused_S = savings[5]
        time_G = genetic[6]; time_S = savings[6]

        shorter.append(distanceS / distanceG) # S is X% longer than G
        avgCust.append(avgCustG / avgCustS)
        vehicles_comp.append(vehiclesS / vehiclesG)
        only_two.append((only_2G, only_2S))
        unused_total.append( unused_S/ unused_G)
        times_t.append(time_G / time_S)
        


    print("Savings needs", sum(vehicles_comp)/len(vehicles_comp), "more vehicles than Genetic")
    print("Savings is", sum(shorter)/len(shorter), "times longer than Genetic")
    print("Genetic has", sum(avgCust)/len(avgCust), "times more customers on each route")

    ct_G = 0
    ct_S = 0
    for (x,y) in only_two:
        if x > y:
            ct_G += 1
        elif x < y:
            ct_S += 1
    
    print("Genetic has", ct_G, "routes with only 2, Savings has", ct_S, "with only 2 customers")
    print("Savings has", sum(unused_total)/len(unused_total), "more unused space")
    print("Genetic is on average", sum(times_t)/len(times_t), "times slower than Savings")

def geneticTime():
    TIMES_GENETIC_FILE = './results/timesGenetic.txt'
    i = 1
    times = []
    f = None
    if (not path.exists(TIMES_GENETIC_FILE)):
        f = open(TIMES_GENETIC_FILE, "w")
        for file in files:
            n_of_customers = search("n(\d+)", file).group(1)
            print("Checkign the time for file", i, "/", all_files)
            i += 1
            time =  None
            try:
                result = subprocess.run(GENETIC_RUN + [file], capture_output=True, text=True, timeout=5)
            except:
                crashed.append(file)
                continue
                
            lines_result = result.stdout.split("\n")
            for line in lines_result:
                if len(line) > 0:
                    if line[0] == "T": # Time
                        time = float(search("([0-9]*[.])?[0-9]+", line).group())

            times.append(time)
            f.write(str(time) + ',' + str(n_of_customers) + '\n')
    else:
        f = open(TIMES_GENETIC_FILE, "r")
        results = []
        for line in f.readlines():
            line = line.split(',')
            results.append((float(line[0]), int(line[1])))
        results.sort(key=lambda x: x[1])
    times = [x[0] for x in results]
    nodes = [x[1] for x in results]
    
    timesGraph = plt.plot(nodes, times)
    plt.ylabel("Ms running")
    plt.xlabel("Nodes")
    plt.title("Time complexity by nodes")
    plt.savefig("genetic Times.png", bbox_inches='tight')
    
    f.close()

def geneticSpace():
    SPACES_GENETIC_FILE = './results/spacesGenetic.txt'
    i = 1
    spaces = []
    results = []
    f = None
    if (not path.exists(SPACES_GENETIC_FILE)):
        f = open(SPACES_GENETIC_FILE, "w")
        for file in files:
            n_of_customers = search("n(\d+)", file).group(1)
            print("Checkign the space for file", i, "/", all_files)
            i += 1
            space =  None
            result = subprocess.run(["valgrind"] + GENETIC_RUN + [file], capture_output=True, text=True, timeout=180)
            
            lines_result = result.stderr.split("\n")
            for line in lines_result:
                if len(line) > 0:
                    if search("bytes allocated", line): # space
                        space = search("(\d+,\d+)+ bytes allocated", line).group()[:-len(("bytes allocated"))]
                        space = space.replace(",", "")
                        spaces.append(float(space))

            spaces.append(space)
            f.write(str(space) + ',' + str(n_of_customers) + '\n')
            results.append((float(space), str(n_of_customers)))
    else:
        f = open(SPACES_GENETIC_FILE, "r")
        results = []
        for line in f.readlines():
            line = line.split(',')
            results.append((float(line[0]), int(line[1])))
        results.sort(key=lambda x: x[1])
    spaces = [x[0] for x in results]
    nodes = [x[1] for x in results]
    
    timesGraph = plt.plot(nodes, spaces)
    plt.ylabel("Heap memory usage")
    plt.xlabel("Nodes")
    plt.title("Space complexity by nodes")
    plt.savefig("genetic Spaces.png", bbox_inches='tight')
    
    f.close()

def savingsTime():
    TIMES_SAVINGS_FILE = './results/timesSavings.txt'
    i = 1
    times = []
    results = []
    f = None
    if (not path.exists(TIMES_SAVINGS_FILE)):
        f = open(TIMES_SAVINGS_FILE, "w")
        for file in files:
            n_of_customers = search("n(\d+)", file).group(1)
            print("Checkign the time for file", i, "/", all_files)
            i += 1
            time =  None
            try:
                result = subprocess.run(SAVINGS_RUN + [file], capture_output=True, text=True, timeout=5)
            except:
                crashed.append(file)
                continue
                
            lines_result = result.stdout.split("\n")
            for line in lines_result:
                if len(line) > 0:
                    if line[0] == "T": # Time
                        time = float(search("([0-9]*[.])?[0-9]+", line).group())

            times.append(time)
            f.write(str(time) + ',' + str(n_of_customers) + '\n')
    else:
        f = open(TIMES_SAVINGS_FILE, "r")
        results = []
        for line in f.readlines():
            line = line.split(',')
            results.append((float(line[0]), int(line[1])))
        results.sort(key=lambda x: x[1])
    times = [x[0] for x in results]
    nodes = [x[1] for x in results]
    
    timesGraph = plt.plot(nodes, times)
    plt.ylabel("Ms running")
    plt.xlabel("Nodes")
    plt.title("Time complexity by nodes")
    plt.savefig("savings Times.png", bbox_inches='tight')
    
    f.close()

def savingsSpace():
    SPACES_GENETIC_FILE = './results/spacesSavings.txt'
    i = 1
    spaces = []
    results = []
    f = None
    if (not path.exists(SPACES_GENETIC_FILE)):
        f = open(SPACES_GENETIC_FILE, "w")
        for file in files:
            n_of_customers = search("n(\d+)", file).group(1)
            print("Checkign the space for file", i, "/", all_files)
            i += 1
            space =  None
            result = subprocess.run(["valgrind"] + SAVINGS_RUN + [file], capture_output=True, text=True, timeout=180)
            
            lines_result = result.stderr.split("\n")
            for line in lines_result:
                if len(line) > 0:
                    if search("bytes allocated", line): # space
                        space = search("(\d+,\d+)+ bytes allocated", line).group()[:-len(("bytes allocated"))]
                        space = space.replace(",", "")
                        spaces.append(float(space))

            spaces.append(space)
            f.write(str(space) + ',' + str(n_of_customers) + '\n')
            results.append((float(space), str(n_of_customers)))
    else:
        f = open(SPACES_GENETIC_FILE, "r")
        results = []
        for line in f.readlines():
            line = line.split(',')
            results.append((float(line[0]), int(line[1])))
        results.sort(key=lambda x: x[1])
    spaces = [x[0] for x in results]
    nodes = [x[1] for x in results]
    
    timesGraph = plt.plot(nodes, spaces)
    plt.ylabel("Heap memory usage")
    plt.xlabel("Nodes")
    plt.title("Space complexity by nodes")
    plt.savefig("savings Spaces.png", bbox_inches='tight')
    
    f.close()

comparisons()
geneticTime()
plt.figure().clear()
geneticSpace()
plt.figure().clear()
savingsSpace()
plt.figure().clear()
savingsTime()

# maybe plot some routes and results later?
