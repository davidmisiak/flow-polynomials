from scripts.flow_poly import Multipole, read_graphs
import networkx as nx
import matplotlib.pyplot as plt


def plot(graphs: list[Multipole], all_layouts=False):
    '''Display the graph (in three different layouts). Outer edges are dashed.'''
    layouts = [
        nx.kamada_kawai_layout,
        nx.spring_layout,
        nx.planar_layout,
    ]
    if not all_layouts:
        layouts = [layouts[0]]
    plt.figure(figsize=(4*len(graphs), 4*len(layouts)))
    for i, graph in enumerate(graphs):
        outer_vertices = sorted(graph.get_outer_vertices())
        inner_vertices = sorted(graph.get_inner_vertices())
        outer_edges = [list(e) for e in graph.outer]
        inner_edges = [list(e) for e in graph.inner]

        G = nx.Graph()
        G.add_nodes_from(outer_vertices + inner_vertices)
        G.add_edges_from(outer_edges + inner_edges)

        node_color = ['g']*len(outer_vertices) + ['k']*len(inner_vertices)

        for j in range(len(layouts)):
            plt.subplot(len(layouts), len(graphs), j*len(graphs)+i+1)
            try:
                pos = layouts[j](G)
                nx.draw_networkx_nodes(G, pos, node_color=node_color)
                nx.draw_networkx_edges(G, pos, edgelist=outer_edges, style='dashed', edge_color='g')
                nx.draw_networkx_edges(G, pos, edgelist=inner_edges)
                nx.draw_networkx_labels(G, pos, font_color='w')
            except nx.NetworkXException:
                # graph is not planar
                pass
    plt.tight_layout()
    plt.show()


def plot_from_files(filenames: list[str]):
    graphs = []
    for filename in filenames:
        with open(filename) as f:
            graphs.append(Multipole.parse(f.read()))
    plot(graphs)


if __name__ == '__main__':
    graphs = list(read_graphs())
    plot(graphs, all_layouts=True)
