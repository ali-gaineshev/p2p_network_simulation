import sys

import networkx as nx
import matplotlib.pyplot as plt

def main():

    graph_type = sys.argv[1]

    match graph_type:
        case "regular":
            regular_graph(sys.argv[2:])

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
    nx.draw(G, with_labels=True, node_color="lightblue", edge_color="gray", node_size=500)

    # Save as an image
    plt.savefig(img_filename, format="png", dpi=300)
    plt.close()
    print(f"Graph saved as {img_filename}")

def save_graph_as_graphml(G, graphml_filename):
    nx.write_graphml(G, graphml_filename)
    print(f"GraphML file saved as {graphml_filename}")

def regular_graph(args):
    d = int(args[0])
    n = int(args[1])
    G = nx.random_regular_graph(d, n)

    edges = list(G.edges)
    write_graph_to_txt(f"{d}_regular_with_{n}_nodes.txt", n, edges)
    save_graph_as_image(G, f"{d}_regular_with_{n}_nodes.png")
    save_graph_as_graphml(G, f"{d}_regular_with_{n}_nodes.graphml")

main()