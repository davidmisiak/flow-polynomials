from flow_poly import Multipole
import networkx as nx
import matplotlib.pyplot as plt

def plot(graph: Multipole):
    '''Display the graph (in three different layouts). Outer edges are dashed.'''

    outer_vertices = sorted(graph.get_outer_vertices())
    inner_vertices = sorted(graph.get_inner_vertices())
    outer_edges = [list(e) for e in graph.outer]
    inner_edges = [list(e) for e in graph.inner]

    G = nx.Graph()
    G.add_nodes_from(outer_vertices + inner_vertices)
    G.add_edges_from(outer_edges + inner_edges)

    plt.figure(figsize=(12, 4))
    layouts = [
        nx.kamada_kawai_layout,
        nx.spring_layout,
        nx.planar_layout,
    ]
    node_color = ['g']*len(outer_vertices) + ['k']*len(inner_vertices)

    for i in range(len(layouts)):
        plt.subplot(1, len(layouts), i+1)
        try:
            pos = layouts[i](G)
            nx.draw_networkx_nodes(G, pos, node_color=node_color)
            nx.draw_networkx_edges(G, pos, edgelist=outer_edges, style='dashed', edge_color='g')
            nx.draw_networkx_edges(G, pos, edgelist=inner_edges)
            nx.draw_networkx_labels(G, pos, font_color='w')
        except nx.NetworkXException:
            # graph is not planar
            pass
    plt.tight_layout()
    plt.show()


if __name__ == '__main__':
    plot(Multipole.read())
