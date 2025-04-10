import sys

import networkx as nx
import matplotlib.pyplot as plt
import random


def main():
    graph_type = sys.argv[1]

    match graph_type:
        case "regular":
            regular_graph(sys.argv[2:])
        case "cluster":
            cluster_graph(sys.argv[2:])
        case "megagraph":
            build_mega_graph(sys.argv[2:])
        case "disable":
            disable_nodes(sys.argv[2:])
        case _:
            raise Exception("Incorrent args")


def write_graph_to_txt(filename, num_nodes, edges):
    with open(filename, "w") as f:
        f.write(f"{num_nodes}\n")  # number of nodes
        for i, (u, v) in enumerate(edges):
            f.write(f"{u} {v}")
            if i != len(edges) - 1:
                f.write("\n")


def save_graph_as_image(G, img_filename):

    plt.figure(figsize=(6, 6))
    nx.draw(G, with_labels=True, node_color="lightblue",
            edge_color="gray", node_size=500)

    # Save as an image
    plt.savefig(img_filename, format="png", dpi=300)
    plt.close()
    print(f"Graph saved as {img_filename}")


def save_graph_as_graphml(G, graphml_filename):
    nx.write_graphml(G, graphml_filename)
    print(f"GraphML file saved as {graphml_filename}")


def save_disabled_nodes_to_txt(filename, disabled_nodes: list):
    with open(filename, "w") as f:
        for i, node in enumerate(disabled_nodes):
            f.write(f"{node}")
            if i != len(disabled_nodes) - 1:
                f.write("\n")


def disable_nodes(args):
    file = args[0]
    n = int(args[1])
    perc = int(args[2])

    count = int(n * perc / 100)
    count = max(0, min(count, n))  # Ensure count is within 0 to n

    numbers = random.sample(range(n), count)
    numbers.sort()
    print(numbers)
    save_disabled_nodes_to_txt(f"{file}_d.txt", numbers)


def regular_graph(args):
    d = int(args[0])
    n = int(args[1])
    G = nx.random_regular_graph(d, n, seed=1)

    edges = list(G.edges)
    write_graph_to_txt(f"{d}_regular_with_{n}_nodes.txt", n, edges)
    save_graph_as_image(G, f"{d}_regular_with_{n}_nodes.png")
    save_graph_as_graphml(G, f"{d}_regular_with_{n}_nodes.graphml")


def cluster_graph(args):
    num_clusters = int(args[0])
    nodes_per_cluster = int(args[1])
    total_cluster_nodes = num_clusters * nodes_per_cluster
    total_nodes = total_cluster_nodes + 1

    G = nx.Graph()
    cluster_representatives = []

    # start clusters from node 1 (for central node to be 0)
    current_node = 1

    # build clusters
    for _ in range(num_clusters):
        cluster_nodes = list(
            range(current_node, current_node + nodes_per_cluster))
        clique = nx.complete_graph(cluster_nodes)
        G.add_edges_from(clique.edges())

        # representative from each cluster connects to node 0
        cluster_representatives.append(cluster_nodes[0])
        current_node += nodes_per_cluster

    # connect node 0 to each representative
    G.add_node(0)
    for rep in cluster_representatives:
        G.add_edge(0, rep)

    edges = list(G.edges())
    write_graph_to_txt(
        f"cluster_{num_clusters}_with_{nodes_per_cluster}_nodes_each.txt", total_nodes, edges)
    save_graph_as_image(
        G, f"cluster_{num_clusters}_with_{nodes_per_cluster}_nodes_each.png")
    save_graph_as_graphml(
        G, f"cluster_{num_clusters}_with_{nodes_per_cluster}_nodes_each.graphml")

 # make the subgraph here and return it (for building mega graphs)


def make_sub_cluster(spec, node_start):
    num_clusters, nodes_per_cluster = spec
    subgraph = nx.Graph()
    representatives = []
    current_node = node_start

    for _ in range(num_clusters):
        nodes = list(range(current_node, current_node + nodes_per_cluster))
        clique = nx.complete_graph(nodes)
        subgraph.add_edges_from(clique.edges())
        representatives.append(nodes[0])
        current_node += nodes_per_cluster

    total_nodes_used = current_node - node_start
    return subgraph, total_nodes_used, representatives

# for mega clusters (multiple clusters connected by a central node 0)


def build_mega_graph(args):
    cluster_specs = []
    for i in range(0, len(args), 2):
        spec = (int(args[i]), int(args[i + 1]))
        cluster_specs.append(spec)

    G = nx.Graph()
    G.add_node(0)
    next_node_id = 1

    for spec in cluster_specs:
        num_clusters, nodes_per_cluster = spec

        hub_node = next_node_id
        next_node_id += 1
        G.add_node(hub_node)
        G.add_edge(0, hub_node)

        for _ in range(num_clusters):
            clique_nodes = list(
                range(next_node_id, next_node_id + nodes_per_cluster))
            clique = nx.complete_graph(clique_nodes)
            G.add_edges_from(clique.edges())
            # connect representative (node 0) to the hub
            G.add_edge(hub_node, clique_nodes[0])
            next_node_id += nodes_per_cluster

    edges = list(G.edges())
    write_graph_to_txt("megagraph.txt", G.number_of_nodes(), edges)
    save_graph_as_image(G, "megagraph.png")
    save_graph_as_graphml(G, "megagraph.graphml")


main()
