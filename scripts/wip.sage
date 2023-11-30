from sage.graphs.graph_coloring import all_graph_colorings
G = Graph()
# G.add_vertices(range(18))
# G.add_edges([(0,1),(0,2),(0,3),(0,4),(0,5),(1,2),(1,5),(2,6),(3,6),(3,7),(4,8),(4,9),(5,9),(6,10),(7,10),(7,11),(8,12),(8,13),(9,13),(10,14),(11,14),(11,15),(12,15),(12,16),(13,16),(14,17),(15,17),(16,17)])
G.add_vertices(range(8))
G.add_edges([(0,5),(0,4),(0,3),(0,2),(0,1),(1,5),(1,6),(2,3),(2,6),(3,4),(4,7),(5,7),(6,7)])
D = G.planar_dual()
save(D.plot(), 'tmp.png')

# print(*D.vertices(), sep='\n')

maps = [
    [1, 2, 3], [1, 2, 4], [1, 3, 4], [2, 3, 4],
]
def remap_coloring(coloring, map_):
    return {map_[c-1]: vs for c, vs in coloring.items()}

colorings = list(all_graph_colorings(D, 4))
colorings3 = list(all_graph_colorings(D, 3))
# colorings += [remap_coloring(c, map_) for c in colorings3 for map_ in maps]

v = [
    ((0, 5), (5, 7), (7, 4), (4, 0)),
    ((0, 1), (1, 5), (5, 0)),
    ((0, 2), (2, 6), (6, 1), (1, 0)),
    ((0, 3), (3, 2), (2, 0)),
    ((0, 4), (4, 3), (3, 0)),
]

def get_vertex_color(coloring, vertex):
    for c, vs in coloring.items():
        if vertex in vs:
            return c-1
    assert False

def get_edge_color(coloring, edge_idx):
    v1_color = get_vertex_color(coloring, v[(edge_idx)%5])
    v2_color = get_vertex_color(coloring, v[(edge_idx+1)%5])
    return ((v1_color - v2_color + 4) % 4) - 1

def get_boudary_color(coloring):
    return [get_edge_color(coloring, i) for i in range(1, 6)]

def rotate_lists(lists, r):
    s = len(lists[0]) - r
    return [l[s:] + l[:s] for l in lists]

allowed = [
    [1, 0, 2, 0, 0],
    [2, 0, 1, 0, 0],
    [0, 1, 2, 1, 1],
    [2, 1, 0, 1, 1],
    [0, 2, 1, 2, 2],
    [1, 2, 0, 2, 2],
]

allowed = [
    [1, 2, 0, 0, 0],
    [2, 1, 0, 0, 0],
    [0, 2, 1, 1, 1],
    [2, 0, 1, 1, 1],
    [0, 1, 2, 2, 2],
    [1, 0, 2, 2, 2],
]

for r in range(5):
    filtered = colorings
    # filtered = [c for c in colorings if get_boudary_color(c) in rotate_lists(allowed, r)]
    print(len(filtered), len(filtered)/4)

# for col in colorings:
#     print(get_boudary_color(col))
