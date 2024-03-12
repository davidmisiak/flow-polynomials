from sage.all import *  # linter hack
import itertools
import datetime


def get_partitions(k):
    for p in sage.combinat.set_partition.SetPartitions(range(k)):
        if p.is_noncrossing() and 1 not in map(len, p):
            yield p


def is_color_parity_correct(cs, indices):
    counts = [0, 0, 0]
    for i in indices:
        counts[cs[i]] += 1
    return counts[0] % 2 == counts[1] % 2 == counts[2] % 2


def is_minimal_coloring(cs):
    i0 = cs.index(0) if 0 in cs else len(cs)
    i1 = cs.index(1) if 1 in cs else len(cs)
    i2 = cs.index(2) if 2 in cs else len(cs)
    return i0 <= i1 <= i2


def get_colorings(k):
    for cs in itertools.product([0, 1, 2], repeat=k):
        if is_color_parity_correct(cs, range(k)) and is_minimal_coloring(cs):
            yield cs


def is_compatible(p, cs):
    return all(is_color_parity_correct(cs, s) for s in p)


for k in range(2, 20):
    print()
    print(datetime.datetime.now(), f'k = {k}')
    partitions = list(get_partitions(k))
    print(datetime.datetime.now(), f'partitions = {len(partitions)}')
    colorings = list(get_colorings(k))
    print(datetime.datetime.now(), f'colorings = {len(colorings)}')

    M = matrix(ZZ, len(colorings), len(partitions))
    for i, cs in enumerate(colorings):
        for j, p in enumerate(partitions):
            M[i, j] = is_compatible(p, cs)
    print(datetime.datetime.now(), '(constructed)')
    rank = M.rank()
    print(datetime.datetime.now(), rank == len(partitions))
