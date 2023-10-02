fset = frozenset
Edge = set[int]


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


class Graph:
    '''
    Represent a k-pole with n vertices.
    Outer vertices must be numbered 0 to k-1 and inner vertices must be numbered k to n-1.
    Each edge should connect either two inner vertices or an inner and an outer vertex.
    '''

    def __init__(self, k: int, n: int, outer: list[Edge], inner: list[Edge]):
        self.k = k
        self.n = n
        self.outer = outer
        self.inner = inner

    def copy(self):
        return Graph(self.k, self.n, self.outer.copy(), self.inner.copy())

    def get_fp(self) -> FP:
        g = self.copy()

        g.inner = [e for e in g.inner if len(e) > 1]
        loop_factor = 3 ** (len(self.inner) - len(g.inner))

        if g.inner == []:
            return FP({g.to_GWIE(): loop_factor})

        e = g.inner.pop()
        fp_removed = g.get_fp()
        g.merge_vertices(*e)
        fp_contracted = g.get_fp()

        return (fp_contracted - fp_removed) * loop_factor

    def merge_vertices(self, u: int, v: int):
        def _replace(x: int, by: int) -> int:
            return by if x == u else x
        self.outer = [{_replace(x, v), _replace(y, v)} for (x, y) in self.outer]
        self.inner = [{_replace(x, v), _replace(y, v)} for (x, y) in self.inner]

    def to_GWIE(self) -> GWIE:
        result: dict[int, set[int]] = {}
        for u, v in self.outer:
            u, v = min(u, v), max(u, v)
            result.setdefault(v, set()).add(u)
        return GWIE(fset(fset(s) for s in result.values()))

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


g = Graph(
    4, 8,
    outer=[{0, 4}, {1, 5}, {2, 6}, {3, 7}],
    inner=[{4, 5}, {5, 6}, {6, 7}, {7, 4}],
)

# g = Graph(
#     3, 10,
#     outer=[{0, 3}, {1, 4}, {2, 5}],
#     inner=[{3, 6}, {6, 4}, {4, 7}, {7, 5}, {5, 8}, {8, 3}, {6, 9}, {7, 9}, {8, 9}],
# )

# g = Graph(
#     0, 10,
#     outer=[],
#     inner=[{i, (i+1)%5} for i in range(5)] + [{i, i+5} for i in range(5)] + [{i+5, ((i+1)%5)+5} for i in range(5)]
# )

# g = Graph(
#     0, 10,
#     outer=[],
#     inner=[{i, (i+1)%5} for i in range(5)] + [{i, i+5} for i in range(5)] + [{i+5, ((i+2)%5)+5} for i in range(5)]
# )

fp = g.get_fp()
print(fp)
print('\nfeasible-only terms', fp.only_feasible())
g.plot()
