import sys
from subprocess import run
from xml.dom import minidom
from dataclasses import dataclass
from typing import List, Tuple
import matplotlib.pyplot as plt

ALGORITHMS = ['savings', 'genetic']
EXIT_FAILURE = 1
EXIT_SUCCESS = 0


@dataclass
class Node:
    id: int
    x: float
    y: float


class Customer(Node):
    pass


class Depot(Node):
    pass


def _load_nodes_from_xml(xml_file: str) -> Tuple[Depot, List[Customer]]:
    """
    Load all nodes representinc customers and the depot from given xml file.
    The function disregards the rest of the information contained in the file.

    params:
        str xml_file: path to the xml file
    returns:
        Tuple[Depot, List[Customers]]: the  depot and customers contained in the file
    """

    file = minidom.parse(xml_file)
    nodes = file.getElementsByTagName('node')

    depot = None
    customers = []
    for node in nodes:
        assert node.attributes['id'].value.isnumeric()
        assert node.attributes['type'].value.isnumeric()
        ch_nodes = []
        for ch_node in node.childNodes:
            if isinstance(ch_node, minidom.Element):
                ch_nodes.append(ch_node)
        assert len(ch_nodes) == 2

        if node.attributes['type'].value == '0':
            depot = Depot(int(node.attributes['id'].value),
                          float(ch_nodes[0].firstChild.data),
                          float(ch_nodes[1].firstChild.data))
        else:
            customers.append(Customer(int(node.attributes['id'].value),
                                      float(ch_nodes[0].firstChild.data),
                                      float(ch_nodes[1].firstChild.data)))
    return depot, customers


def _generate_paths(algorithm: str, data_file: str, executable: str = './gal'):
    """
    Generates paths with given algorithm and dataset. Expects the 'gal' executable
    in the same dirrectory by default.

    params:
        str algorithm: one of the implemented algorithms ['savings'|'genetic']
        str data_file: path to the xml data file that will be provided to the specified executable
        str executable: path to executable of the project (by defualt ./gal)
    returns:
        List[List[int]]: paths represented as list of customer IDs
    """
    if algorithm not in ALGORITHMS:
        print('[E]: Couldn\'t generate paths. Wrong algorithm specified', file=sys.stderr)
        exit(EXIT_FAILURE)

    try:
        result = run([executable, '--algorithm', algorithm, data_file],
                     capture_output=True, text=True, timeout=5)
    except Exception:
        print(f'[E]: Couldn\'t generate paths. The {executable} timed out or crashed.', file=sys.stderr)
        exit(EXIT_FAILURE)

    # Following is specific output parsing
    # The output contains:
    #       N paths in format #<path-number>(<space><customer-number>)*
    #       some generic information about the paths (these are not significant and are skipped)
    result = result.stdout.split('\n')
    paths = []
    for line in result:
        if line.startswith('#'):
            # strips the #<path-number>
            path = line.strip().split(' ')[1:]
            paths.append(int(id) for id in path)
    return paths


def _convert_path_to_positions(path: List[int], customers: List[Customer]) -> Tuple[List[int], List[int]]:
    x_path = []
    y_path = []
    for customer_id in path:
        # expects sorted list of customers by their id
        customer = customers[customer_id - 2]
        x_path.append(customer.x)
        y_path.append(customer.y)

    return x_path, y_path


def plot_paths(algorithm: str, xml_file: str):
    id_paths = _generate_paths(algorithm, xml_file)
    print(id_paths)
    depot, customers = _load_nodes_from_xml(xml_file)

    for path in id_paths:
        xs, ys = _convert_path_to_positions(path, customers)
        xs = [depot.x] + xs + [depot.x]
        ys = [depot.y] + ys + [depot.y]
        plt.plot(xs, ys, marker='o')
    plt.show()


algorithm = sys.argv[1]
xml_file = sys.argv[2]
plot_paths(algorithm, xml_file)
