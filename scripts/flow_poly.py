from functools import lru_cache
from dataclasses import dataclass

fset = frozenset
Edge = fset[int]
Edges = tuple[Edge, ...]


class Partition(fset[fset[int]]):
    '''
    Partition of vertex set.
    '''

    def __repr__(self) -> str:
        return ''.join('[ ' + ' '.join(str(v) for v in g)  + ' ]' for g in self.to_sorted())

    def to_sorted(self) -> list[list[int]]:
        return sorted(sorted(g) for g in self)


class FlowPoly(dict[Partition, int]):
    '''
    Flow polynomial expressed as a linear combination of its multipole minors without inner edges
    (each represented by the corresponding partition of its vertex set).
    '''

    def __sub__(self, other: 'FlowPoly') -> 'FlowPoly':
        result = FlowPoly()
        for p in self.keys() | other.keys():
            result[p] = self.get(p, 0) - other.get(p, 0)
        return FlowPoly({p: count for p, count in result.items() if count})

    def __mul__(self, k: int) -> 'FlowPoly':
        if k == 0:
            return FlowPoly()
        result = FlowPoly()
        for p in self.keys():
            result[p] = self[p] * k
        return result

    def __repr__(self) -> str:
        items = sorted(self.items(), key=lambda x: x[0].to_sorted())
        return '[\n' + ''.join(f'{count:4d} * {p}\n' for p, count in items) + ']'

    def prune(self) -> 'FlowPoly':
        '''Remove multipoles that can not be compatible with any boundary value.'''
        return FlowPoly({p: count for p, count in self.items() if 1 not in map(len, p)})


@dataclass(frozen=True, unsafe_hash=True)
class Multipole:
    '''
    Represent a k-pole with n inner vertices.
    Outer vertices must be assigned negative numbers and inner vertices non-negative numbers.
    Each edge should connect either two inner vertices or an inner and an outer vertex.
    '''

    outer: Edges
    inner: Edges

    @staticmethod
    def build(edges: list[tuple[int, ...]]) -> 'Multipole':
        outer, inner = [], []
        for u, v in edges:
            if min(u, v) < 0:
                outer.append(Edge({u, v}))
            else:
                inner.append(Edge({u, v}))
        return Multipole(tuple(outer), tuple(inner))

    @staticmethod
    def read() -> 'Multipole':
        m = int(input())
        edges = [tuple(map(int, input().split())) for _ in range(m)]
        return Multipole.build(edges)

    def get_outer_vertices(self) -> set[int]:
        return {v for e in self.outer for v in e if v < 0}

    def get_inner_vertices(self) -> set[int]:
        return {v for e in self.outer+self.inner for v in e if v >= 0}

    def remove_loops(self) -> tuple['Multipole', int]:
        inner, loops = [], 0
        for e in self.inner:
            if len(e) > 1:
                inner.append(e)
            else:
                loops += 1
        return Multipole(self.outer, tuple(inner)), loops

    def pop_inner_edge(self) -> tuple['Multipole', Edge]:
        e = self.inner[-1]
        g = Multipole(self.outer, self.inner[:-1])
        return g, e

    def merge_vertices(self, u: int, v: int) -> 'Multipole':
        def _replace(x: int, by: int) -> int:
            return by if x == u else x
        outer = tuple(Edge({_replace(x, v), _replace(y, v)}) for (x, y) in self.outer)
        inner = tuple(Edge({_replace(x, v), _replace(y, v)}) for (x, y) in self.inner)
        return Multipole(outer, inner)

    def to_partition(self) -> Partition:
        result: dict[int, set[int]] = {}
        for e in self.outer:
            u, v = min(e), max(e)
            result.setdefault(v, set()).add(u)
        return Partition(fset(fset(s) for s in result.values()))

    @lru_cache
    def get_flow_poly(self) -> FlowPoly:
        g, loops = self.remove_loops()
        loop_factor = 3 ** loops

        if len(g.inner) == 0:
            return FlowPoly({g.to_partition(): loop_factor})

        g, e = g.pop_inner_edge()
        result_removed = g.get_flow_poly()
        g = g.merge_vertices(*e)
        result_contracted = g.get_flow_poly()

        return (result_contracted - result_removed) * loop_factor


if __name__ == '__main__':
    g = Multipole.read()
    fp = g.get_flow_poly()
    # print(fp)
    print(fp.prune())
