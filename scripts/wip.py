from flow_poly import fset, Multipole
from itertools import product, permutations
import math

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


# for k in range(3, 9):
#     compute_partition_counts(k)



def do_colors_sum_to_zero(vals):
    vals = [[(0, 1), (1, 0), (1, 1)][v] for v in vals]
    first = sum(v[0] for v in vals) % 2
    second = sum(v[1] for v in vals) % 2
    return first == second == 0

def canonicalize_coloring(cs):
    rotations = []
    for i in range(len(cs)):
        rotations.append(tuple(cs[i:] + cs[:i]))
    all_reprs = []
    for perm in permutations(range(3)):
        for rot in rotations:
            all_reprs.append(tuple(perm[c] for c in rot))
    return min(all_reprs)

def compute_colorings(k):
    colorings = set()
    for cs in product([0, 1, 2], repeat=k):
        if do_colors_sum_to_zero(cs):
            colorings.add(canonicalize_coloring(cs))
    return sorted(colorings)

k4_partitions = [
    [[0,1,2,3]],[[0,1],[2,3]],[[1,2],[3,0]],
]

k5_partitions = [
    [[0,1,2,3,4]],[[0,1],[2,3,4]],[[1,2],[3,4,0]],[[2,3],[4,0,1]],[[3,4],[0,1,2]],[[4,0],[1,2,3]],
]

k6_partitions = [
    [[0,1,2,3,4,5]],[[0,1],[2,3,4,5]],[[1,2],[3,4,5,0]],[[2,3],[4,5,0,1]],[[3,4],[5,0,1,2]],
    [[4,5],[0,1,2,3]],[[5,0],[1,2,3,4]],[[0,1,2],[3,4,5]],[[1,2,3],[4,5,0]],[[2,3,4],[5,0,1]],
    [[0,1],[2,5],[3,4]],[[1,2],[3,0],[4,5]],[[2,3],[4,1],[5,0]],[[0,1],[2,3],[4,5]],[[1,2],[3,4],[5,0]],
]

k7_partitions = [
    [[0,1,2,3,4,5,6]],[[0,1],[2,3,4,5,6]],[[1,2],[3,4,5,6,0]],[[2,3],[4,5,6,0,1]],
    [[3,4],[5,6,0,1,2]],[[4,5],[6,0,1,2,3]],[[5,6],[0,1,2,3,4]],[[6,0],[1,2,3,4,5]],
    [[0,1,2],[3,4,5,6]],[[1,2,3],[4,5,6,0]],[[2,3,4],[5,6,0,1]],[[3,4,5],[6,0,1,2]],
    [[4,5,6],[0,1,2,3]],[[5,6,0],[1,2,3,4]],[[6,0,1],[2,3,4,5]],[[0,1],[2,3],[4,5,6]],
    [[1,2],[3,4],[5,6,0]],[[2,3],[4,5],[6,0,1]],[[3,4],[5,6],[0,1,2]],[[4,5],[6,0],[1,2,3]],
    [[5,6],[0,1],[2,3,4]],[[6,0],[1,2],[3,4,5]],[[0,1],[2,6],[3,4,5]],[[1,2],[3,0],[4,5,6]],
    [[2,3],[4,1],[5,6,0]],[[3,4],[5,2],[6,0,1]],[[4,5],[6,3],[0,1,2]],[[5,6],[0,4],[1,2,3]],
    [[6,0],[1,5],[2,3,4]],[[0,1],[3,4],[2,5,6]],[[1,2],[4,5],[3,6,0]],[[2,3],[5,6],[4,0,1]],
    [[3,4],[6,0],[5,1,2]],[[4,5],[0,1],[6,2,3]],[[5,6],[1,2],[0,3,4]],[[6,0],[2,3],[1,4,5]],
]

def compute_k4_sym_sum(coefs):
    assert len(coefs) == 3
    return [
        2*(coefs[0]),
        1*(coefs[1]+coefs[2]),
    ]

def compute_k5_sym_sum(coefs):
    assert len(coefs) == 6
    return [
        5*(coefs[0]),
        1*(coefs[1]+coefs[2]+coefs[3]+coefs[4]+coefs[5]),
    ]

def compute_k6_sym_sum(coefs):
    assert len(coefs) == 15
    return [
        6*(coefs[0]),
        1*(coefs[1]+coefs[2]+coefs[3]+coefs[4]+coefs[5]+coefs[6]),
        2*(coefs[7]+coefs[8]+coefs[9]),
        2*(coefs[10]+coefs[11]+coefs[12]),
        3*(coefs[13]+coefs[14]),
    ]

def compute_k7_sym_sum(coefs):
    assert len(coefs) == 36
    return [
        7*(coefs[0]),
        1*(coefs[1]+coefs[2]+coefs[3]+coefs[4]+coefs[5]+coefs[6]+coefs[7]),
        1*(coefs[8]+coefs[9]+coefs[10]+coefs[11]+coefs[12]+coefs[13]+coefs[14]),
        1*(coefs[15]+coefs[16]+coefs[17]+coefs[18]+coefs[19]+coefs[20]+coefs[21]),
        1*(coefs[22]+coefs[23]+coefs[24]+coefs[25]+coefs[26]+coefs[27]+coefs[28]),
        1*(coefs[29]+coefs[30]+coefs[31]+coefs[32]+coefs[33]+coefs[34]+coefs[35]),
    ]

partitions_and_sum_fns = {
    4: (k4_partitions, compute_k4_sym_sum),
    5: (k5_partitions, compute_k5_sym_sum),
    6: (k6_partitions, compute_k6_sym_sum),
    7: (k7_partitions, compute_k7_sym_sum),
}

def compute_coef(partition, cs):
    for s in partition:
        group = [cs[i] for i in s]
        if not do_colors_sum_to_zero(group):
            return 0
    return 1

for k in [4, 5, 6, 7]:
    partitions, sum_fn = partitions_and_sum_fns[k]
    ss_partition_count = len(sum_fn([0 for _ in partitions]))
    colorings = compute_colorings(k)
    print(f'k={k}, partitions={len(partitions)} ({ss_partition_count}), colorings={len(colorings)}')
    sym_sums = []
    for cs in colorings:
        print(cs)
        coefs = [compute_coef(p, cs) for p in partitions]
        sym_sum = sum_fn(coefs)
        sym_sum = [s // math.gcd(*sym_sum) for s in sym_sum]
        sym_sums.append(sym_sum)
    sym_sums = sorted(sym_sums, key=lambda x: (x[0], sum(x), x))
    for s in sym_sums:
        print(s)
