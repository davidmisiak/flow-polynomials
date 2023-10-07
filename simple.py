from itertools import product
from functools import lru_cache
from dataclasses import dataclass


fset = frozenset
Edge = fset[int]
Edges = tuple[Edge, ...]


class GWIE(fset[fset[int]]):
    '''
    Multipole without inner edges, represented by the sets of outer vertices with a common inner neighbor.
    '''

    def __repr__(self) -> str:
        return '[' + ']['.join(' '.join(str(v) for v in g) for g in self.to_sorted()) + ']'

    def to_sorted(self) -> list[list[int]]:
        return sorted(sorted(g) for g in self)


class FP(dict[GWIE, int]):
    '''
    Flow polynomial expressed as a linear combination of multipoles without inner edges.
    '''

    def __sub__(self, other: 'FP') -> 'FP':
        result = FP()
        for gwie in self.keys() | other.keys():
            result[gwie] = self.get(gwie, 0) - other.get(gwie, 0)
        return FP({gwie: count for gwie, count in result.items() if count})

    def __mul__(self, k: int) -> 'FP':
        if k == 0:
            return FP()
        result = FP()
        for gwie in self.keys():
            result[gwie] = self[gwie] * k
        return result

    def __repr__(self) -> str:
        items = sorted(self.items(), key=lambda x: x[0].to_sorted())
        return 'FP:\n' + '\n'.join(f'{count:4d} * {gwie}' for gwie, count in items)

    def only_feasible(self) -> 'FP':
        '''Remove multipoles that can not be compatible with any boundary value.'''
        return FP({gwie: count for gwie, count in self.items() if 1 not in map(len, gwie)})


@dataclass(frozen=True, unsafe_hash=True)
class Graph:
    '''
    Represent a k-pole with n vertices.
    Outer vertices must be numbered 0 to k-1 and inner vertices must be numbered k to n-1.
    Each edge should connect either two inner vertices or an inner and an outer vertex.
    '''

    k: int
    n: int
    outer: Edges
    inner: Edges

    def remove_loops(self) -> tuple['Graph', int]:
        inner, loops = [], 0
        for e in self.inner:
            if len(e) > 1:
                inner.append(e)
            else:
                loops += 1
        return Graph(self.k, self.n, self.outer, tuple(inner)), loops

    def pop_inner_edge(self) -> tuple['Graph', Edge]:
        e = self.inner[-1]
        g = Graph(self.k, self.n, self.outer, self.inner[:-1])
        return g, e

    def merge_vertices(self, u: int, v: int) -> 'Graph':
        def _replace(x: int, by: int) -> int:
            return by if x == u else x
        outer = tuple(Edge({_replace(x, v), _replace(y, v)}) for (x, y) in self.outer)
        inner = tuple(Edge({_replace(x, v), _replace(y, v)}) for (x, y) in self.inner)
        return Graph(self.k, self.n, outer, inner)

    def to_GWIE(self) -> GWIE:
        result: dict[int, set[int]] = {}
        for u, v in self.outer:
            u, v = min(u, v), max(u, v)
            result.setdefault(v, set()).add(u)
        return GWIE(fset(fset(s) for s in result.values()))

    @lru_cache
    def get_fp(self) -> FP:
        g, loops = self.remove_loops()
        loop_factor = 3 ** loops

        if len(g.inner) == 0:
            return FP({g.to_GWIE(): loop_factor})

        g, e = g.pop_inner_edge()
        fp_removed = g.get_fp()
        g = g.merge_vertices(*e)
        fp_contracted = g.get_fp()

        return (fp_contracted - fp_removed) * loop_factor

    def plot(self):
        '''Display the graph (in three different layouts). Outer edges are dashed.'''
        import networkx as nx
        import matplotlib.pyplot as plt

        G = nx.Graph()
        G.add_nodes_from(range(self.n))
        G.add_edges_from(self.inner)
        G.add_edges_from(self.outer)

        plt.figure(figsize=(12, 4))
        layouts = [
            nx.kamada_kawai_layout,
            nx.spring_layout,
            nx.planar_layout,
        ]
        for i in range(len(layouts)):
            plt.subplot(1, len(layouts), i+1)
            try:
                pos = layouts[i](G)
                nx.draw_networkx_nodes(G, pos, node_color=['g']*self.k + ['k']*(self.n-self.k))
                nx.draw_networkx_edges(G, pos, edgelist=[list(e) for e in g.inner])
                nx.draw_networkx_edges(G, pos, edgelist=[list(e) for e in g.outer], style='dashed', edge_color='g')
                nx.draw_networkx_labels(G, pos, font_color='w')
            except nx.NetworkXException:
                # graph is not planar
                pass
        plt.tight_layout()
        plt.show()


def compute_gwie_counts():
    import networkx as nx

    for N in range(3, 9):
        gwies = set()
        for blueprint in product(range(N, 2*N-2), repeat=N):
            el = tuple(fset({i, neighbor}) for i, neighbor in enumerate(blueprint))
            gwie = Graph(N, 2*N, el, ()).to_GWIE()
            if gwie in gwies:
                continue
            nxg = nx.Graph()
            nxg.add_edges_from(el)
            nxg.add_edges_from([(i, -1) for i in range(N)])
            nxg.add_edges_from([(i, (i+1)%N) for i in range(N)])
            if nx.check_planarity(nxg)[0]:
                gwies.add(gwie)
        feas_gwies = sum(1 not in map(len, gwie) for gwie in gwies)
        print(N, len(gwies)+1+N*(N-1)//2, feas_gwies)


def make_graph(outer: list[set[int]], inner: list[set[int]]) -> Graph:
    k = max([min(e) for e in outer], default=0) + 1
    n = max([max(e) for e in outer + inner], default=0) + 1
    outer_ = tuple(Edge(e) for e in outer)
    inner_ = tuple(Edge(e) for e in inner)
    return Graph(k, n, outer_, inner_)


g = make_graph(
    [{0, 4}, {1, 5}, {2, 6}, {3, 7}],
    [{4, 5}, {5, 6}, {6, 7}, {7, 4}],
)

# g = make_graph(
#     [{0, 3}, {1, 4}, {2, 5}],
#     [{3, 6}, {6, 4}, {4, 7}, {7, 5}, {5, 8}, {8, 3}, {6, 9}, {7, 9}, {8, 9}],
# )

# g = make_graph(
#     [],
#     [{i, (i+1)%5} for i in range(5)] + [{i, i+5} for i in range(5)] + [{i+5, ((i+1)%5)+5} for i in range(5)]
# )

# g = make_graph(
#     [],
#     [{i, (i+1)%5} for i in range(5)] + [{i, i+5} for i in range(5)] + [{i+5, ((i+2)%5)+5} for i in range(5)]
# )

# g = make_graph(
#     [{0, 2}, {1, 11}],
#     [{2, 3}, {2, 4}, {3, 5}, {3, 6}, {4, 7}, {4, 8}, {5, 6}, {7, 8}, {5, 9}, {6, 9}, {7, 10}, {8, 10}, {9, 11}, {10, 11}],
# )

# g = make_graph(
#     [{0, 2}, {1, 23}],
#     [{2, 3}, {2, 4}, {3, 5}, {3, 6}, {4, 7}, {4, 8}, {5, 9}, {5, 10}, {6, 11}, {6, 12}, {7, 13}, {7, 14}, {8, 15}, {8, 16}, {9, 10}, {11, 12}, {13, 14}, {15, 16}, {9, 17}, {10, 17}, {11, 18}, {12, 18}, {13, 19}, {14, 19}, {15, 20}, {16, 20}, {17, 21}, {18, 21}, {19, 22}, {20, 22}, {21, 23}, {22, 23}],
# )

# g = make_graph(
#     [{i, i+16} for i in range(8)] + [{i+8, i+16} for i in range(8)],
#     [{16, 17}, {18, 19}, {20, 21}, {22, 23}]
# )

fp = g.get_fp()
print(fp)
print('\nfeasible-only terms', fp.only_feasible())
g.plot()

# compute_gwie_counts()
