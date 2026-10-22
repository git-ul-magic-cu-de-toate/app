import random

def generate_graph(num_vertices, max_edges, max_cost):
    """
    Generates a graph in the specified format.
    
    Args:
        num_vertices (int): Number of vertices in the graph.
        max_edges (int): Maximum number of edges for each vertex.
        max_cost (int): Maximum cost of an edge.

    Returns:
        str: A formatted graph string.
    """
    graph = []
    graph.append(f"{num_vertices}")
    
    for u in range(num_vertices):
        num_edges = random.randint(1, max_edges)
        connected_vertices = random.sample(range(num_vertices), num_edges)
        for v in connected_vertices:
            if u != v:
                cost = random.randint(1, max_cost)
                graph.append(f"{u} {v} {cost}")
    
    return "\n".join(graph)

NUM_VERTICES = 100000
MAX_EDGES = 12
MAX_COST = 56  

graph_input = generate_graph(NUM_VERTICES, MAX_EDGES, MAX_COST)

print(graph_input)

with open("in_files/graph3.in", "w") as file:
    file.write(graph_input)
