from flow_poly import fset, Multipole
from itertools import product, permutations
import math
import numpy as np

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



def find_lin_comb(vectors, target):
    import scipy
    c = np.array([1 for _ in vectors])
    A_eq = np.array(vectors).T
    b_eq = np.array(target)
    return scipy.optimize.linprog(c, A_eq=A_eq, b_eq=b_eq, bounds=(0, None))

vectors = [
    [1, 0, 1, 0, 0],
    [1, 1, 0, 1, 1],
    [2, 1, 0, 0, 1],
    [3, 1, 0, 0, 0],
    [3, 0, 2, 0, 0],
    [3, 1, 0, 1, 0],
    [3, 1, 1, 1, 0],
    [4, 2, 0, 0, 1],
    [4, 2, 1, 1, 1],
    [9, 3, 2, 1, 3],
]
targets = [
    [6, 4, 0, 2, 3],
    [6, 1, 2, 0, 0],
]

vectors = [
    [4, 1, 1, 0, 0, 1],
    [4, 2, 1, 1, 1, 0],
    [6, 2, 1, 0, 1, 1],
    [7, 0, 3, 0, 0, 0],
    [7, 2, 1, 0, 0, 1],
    [7, 2, 1, 0, 1, 1],
    [7, 3, 0, 0, 0, 2],
    [7, 3, 1, 1, 1, 0],
    [7, 2, 3, 1, 1, 1],
    [7, 2, 3, 1, 2, 0],
    [7, 3, 2, 1, 2, 1],
    [7, 4, 1, 1, 0, 3],
    [7, 4, 2, 2, 2, 1],
    [8, 3, 2, 1, 0, 2],
    [9, 3, 1, 0, 1, 1],
    [9, 4, 2, 1, 2, 2],
    [9, 4, 3, 2, 3, 0],
    [10, 4, 3, 2, 3, 0],
    [11, 3, 3, 1, 1, 0],
    [11, 4, 3, 1, 0, 3],
    [11, 4, 3, 1, 1, 3],
    [11, 5, 3, 2, 1, 3],
    [12, 4, 3, 1, 3, 0],
    [12, 5, 4, 2, 3, 1],
    [12, 6, 4, 3, 4, 1],
    [13, 3, 3, 0, 0, 3],
    [13, 7, 2, 2, 2, 4],
    [14, 3, 3, 0, 1, 2],
    [14, 5, 1, 0, 1, 2],
    [14, 3, 5, 1, 0, 2],
    [14, 5, 5, 2, 2, 2],
    [14, 6, 4, 3, 2, 2],
    [15, 5, 4, 1, 4, 1],
    [16, 6, 2, 1, 0, 2],
    [17, 8, 2, 2, 0, 3],
    [18, 8, 5, 3, 3, 2],
    [19, 6, 5, 2, 0, 4],
    [21, 5, 4, 0, 0, 4],
    [21, 9, 1, 1, 0, 4],
    [21, 9, 2, 2, 1, 2],
    [21, 8, 7, 3, 6, 1],
    [21, 10, 5, 3, 2, 5],
    [22, 2, 9, 1, 1, 1],
    [22, 6, 4, 0, 4, 1],
    [22, 10, 3, 3, 1, 2],
    [22, 8, 9, 4, 8, 0],
    [23, 14, 5, 5, 3, 9],
    [24, 11, 7, 4, 4, 4],
    [25, 7, 5, 0, 5, 2],
    [25, 8, 9, 3, 8, 0],
    [26, 10, 5, 3, 1, 2],
    [28, 10, 7, 5, 3, 0],
    [28, 11, 4, 3, 2, 5],
    [29, 16, 9, 7, 7, 7],
    [30, 2, 12, 1, 2, 0],
    [30, 10, 11, 4, 10, 0],
    [31, 7, 13, 3, 3, 4],
    [32, 12, 3, 1, 2, 4],
    [33, 13, 4, 2, 2, 4],
    [35, 7, 8, 0, 7, 0],
    [35, 7, 9, 0, 7, 0],
    [35, 13, 7, 5, 3, 0],
    [35, 12, 6, 3, 4, 5],
    [35, 15, 3, 3, 1, 8],
    [35, 18, 9, 9, 7, 5],
    [36, 12, 10, 3, 10, 1],
    [38, 10, 9, 1, 1, 9],
    [38, 10, 15, 4, 3, 6],
    [39, 16, 14, 8, 14, 0],
    [42, 16, 3, 1, 2, 6],
    [42, 10, 9, 0, 1, 9],
    [42, 18, 5, 5, 0, 4],
    [42, 18, 15, 9, 15, 1],
    [45, 24, 7, 7, 5, 15],
    [47, 18, 19, 9, 17, 1],
    [49, 16, 13, 8, 3, 3],
    [49, 19, 9, 4, 1, 15],
    [49, 16, 16, 5, 15, 1],
    [49, 24, 17, 11, 15, 7],
    [52, 18, 10, 5, 0, 4],
    [55, 22, 20, 11, 20, 0],
    [56, 22, 13, 11, 9, 0],
    [56, 24, 12, 12, 7, 2],
    [57, 25, 9, 7, 3, 6],
    [61, 21, 17, 8, 3, 13],
    [62, 24, 12, 9, 6, 0],
    [63, 17, 15, 2, 15, 0],
    [63, 28, 10, 8, 3, 7],
    [63, 30, 5, 5, 1, 21],
    [63, 32, 9, 9, 7, 19],
    [65, 24, 26, 12, 24, 0],
    [75, 34, 11, 9, 2, 11],
    [76, 18, 18, 1, 16, 0],
    [76, 26, 22, 13, 8, 8],
    [77, 19, 16, 0, 0, 18],
    [77, 20, 23, 10, 3, 3],
    [82, 33, 29, 14, 24, 4],
    [84, 29, 25, 9, 24, 1],
    [86, 38, 17, 13, 9, 6],
    [89, 35, 17, 8, 3, 27],
    [91, 28, 25, 14, 6, 6],
    [98, 42, 21, 21, 13, 2],
    [105, 38, 26, 19, 16, 5],
    [105, 50, 11, 11, 7, 31],
    [106, 46, 17, 17, 9, 2],
    [122, 54, 21, 17, 9, 10],
    [123, 41, 43, 15, 39, 1],
    [124, 54, 23, 18, 11, 8],
    [140, 46, 37, 23, 27, 0],
    [146, 54, 36, 27, 22, 8],
    [147, 63, 23, 23, 11, 3],
    [161, 68, 26, 25, 12, 3],
    [170, 74, 27, 21, 7, 18],
    [189, 75, 35, 17, 7, 57],
    [189, 84, 29, 20, 13, 57],
    [196, 72, 48, 36, 29, 10],
    [212, 90, 34, 33, 16, 4],
    [224, 93, 36, 27, 22, 47],
    [287, 102, 41, 20, -1, 62],
    [371, 172, 51, 44, 35, 103],
    [410, 182, 65, 53, 21, 42],
    [421, 163, 58, 32, -2, 58],
    [769, 250, 125, -1, 110, 125],
    [798, 287, 54, -1, 41, 134],
    [833, 269, 140, -1, 118, 140],
]
targets = [
    # [7, 0, 3, 0, 0, 0],
    [7, 1, 2, 0, 1, 0],
    # [7, 2, 1, 0, 0, 1],
    # [7, 3, 0, 0, 0, 2],
    [7, 2, 2, 1, 1, 0],
    [7, 3, 1, 1, 0, 1],
    # [7, 3, 1, 1, 1, 0],
    # [7, 2, 3, 1, 2, 0],
    # [7, 4, 1, 1, 0, 3],
    # [7, 4, 2, 2, 2, 1],
]

# for target in targets:
#     result = find_lin_comb(vectors, target)
#     print(result.success, result.x)
