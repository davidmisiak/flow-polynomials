from flow_poly import fset, Multipole
from itertools import product

def compute_partition_counts(k):
    import networkx as nx

    partitions = set()
    for proto in product(range(k-2), repeat=k):
        edges = tuple(fset({-i-1, neighbor}) for i, neighbor in enumerate(proto))
        p = Multipole(edges, ()).to_partition()
        if p in partitions:
            continue
        nxg = nx.Graph()
        nxg.add_edges_from(edges)
        nxg.add_edges_from([(-i-1, k) for i in range(k)])
        nxg.add_edges_from([(-i-1, -((i+1)%k)-1) for i in range(k)])
        if nx.check_planarity(nxg)[0]:
            partitions.add(p)
    feas_partitions = sum(1 not in map(len, p) for p in partitions)
    print(k, len(partitions)+1+k*(k-1)//2, feas_partitions)


for k in range(3, 9):
    compute_partition_counts(k)
