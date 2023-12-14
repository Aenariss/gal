import sys
from subprocess import run
from re import search
from os import listdir

ALGORITHMS = ['savings', 'genetic']
EXIT_FAILURE = 1


def _gather_experiment_data(algorithm: str, data_file: str, executable: str = './gal'):
    if algorithm not in ALGORITHMS:
        print('[E]: Couldn\'t gather data. Wrong algorithm specified', file=sys.stderr)
        exit(EXIT_FAILURE)

    try:
        print(f"[I] Running {executable} --algorithm {algorithm} {data_file}")
        result = run([executable, '--algorithm', algorithm, data_file],
                     capture_output=True, text=True, timeout=120)
    except Exception:
        print(f'[E]: Couldn\'t gather data. The {executable} timed out or crashed.', file=sys.stderr)
        exit(EXIT_FAILURE)

    try:
        print(f"[I] Running valgrind {executable} --algorithm {algorithm} {data_file}")
        memory_result = run(['valgrind', executable, '--algorithm', algorithm, data_file],
                            capture_output=True, text=True, timeout=5 * 120)
    except Exception:
        print(f'[E]: Couldn\'t gather memory data. The {executable} timed out or crashed.', file=sys.stderr)
        exit(EXIT_FAILURE)

    # Following is specific output parsing
    # The output contains:
    #       N paths in format #<path-number>(<space><customer-number>)*  [skipped]
    #       statistical information about the result in format <description> <value> <optional-units>
    result = result.stdout.strip().split('\n')
    values = []
    for line in result:
        if not line.startswith('#'):
            line = line.strip().split(' ')
            if line[-1].isnumeric() or '.' in line[-1]: # hacky way to distingush if optional units are defined
                value = float(line[-1])
            else:
                value = float(line[-2])
            values.append(value)

    # The second run is memory profiler
    # We extract only the total memory allocated and add it to the values we want to gather
    memory_result = memory_result.stderr.strip().split("\n")
    for line in memory_result:
        if len(line) > 0:
            if search("bytes allocated", line): # space
                space = search(r"(\d+,\d+)+ bytes allocated", line).group()[:-len(("bytes allocated"))]
                space = space.replace(",", "")
                values.append(float(space))

    return values


algorithm = sys.argv[1]
folder_with_xml_graphs = sys.argv[2]
output_file = sys.argv[3]

files = [folder_with_xml_graphs.rstrip('/') + '/' + x for x in listdir(folder_with_xml_graphs) if x.endswith(".xml")]
for i, file in enumerate(files):
    n_of_customers = search(r"n(\d+)", file).group(1)
    print(f"[{i+1}/{len(files)}] Running {algorithm} on {file} with {n_of_customers} customers.")
    values = _gather_experiment_data(algorithm, file)
    with open(output_file, 'a') as of:
        of.write(','.join([n_of_customers] + [str(value) for value in values]) + '\n')

