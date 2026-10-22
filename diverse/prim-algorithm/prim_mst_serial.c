#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int** adj_matrix;
    int nr_vertices;
} Graph;

// Function to find the minimum key value
int minKey(int key[], int mstSet[], int nr_vertices) {
    int min = INT_MAX, min_index;

    for (int v = 0; v < nr_vertices; v++)
        if (mstSet[v] == 0 && key[v] < min)
            min = key[v], min_index = v;

    return min_index;
}

// Prim's algorithm
void primMST(Graph* graph) {
    int* parent = (int*)malloc(graph->nr_vertices * sizeof(int));
    int* key = (int*)malloc(graph->nr_vertices * sizeof(int));
    int* mstSet = (int*)malloc(graph->nr_vertices * sizeof(int));

    for (int i = 0; i < graph->nr_vertices; i++)
        key[i] = INT_MAX, mstSet[i] = 0;

    key[0] = 0;
    parent[0] = -1;

    for (int count = 0; count < graph->nr_vertices - 1; count++) {
        int u = minKey(key, mstSet, graph->nr_vertices);
        mstSet[u] = 1;

        for (int v = 0; v < graph->nr_vertices; v++) {
            if (graph->adj_matrix[u][v] && mstSet[v] == 0 && graph->adj_matrix[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph->adj_matrix[u][v];
            }
        }
    }

    // Used for testing the corectness of the implementation between different serial and parallel approaches
    // FILE* out_file = fopen("testing_outputs/serial_mst_output.txt", "w");
    // if (out_file == NULL) {
    //     printf("Could not open output file.\n");
    //     free(parent);
    //     free(key);
    //     free(mstSet);
    //     return;
    // }

    // fprintf(out_file, "Edge Weight\n");
    // for (int i = 1; i < graph->nr_vertices; i++) {
    //     fprintf(out_file, "%d - %d %d\n", parent[i], i, graph->adj_matrix[i][parent[i]]);
    // }

    // fclose(out_file);

    free(parent);
    free(key);
    free(mstSet);
}

// Read graph from file
Graph* read_graph_from_file() {
    Graph* struct_graph = (Graph*)malloc(sizeof(Graph));

    char* in_files = "in_files/graph2.in";

    FILE* fp;
    fp = fopen(in_files, "r");

    if (fp == NULL) {
        printf("Could not open file %s", in_files);
        return NULL;
    }

    int nr_vertices;
    fscanf(fp, "%d", &nr_vertices);

    int** adj_matrix = (int**)malloc(nr_vertices * sizeof(int*));
    for (int i = 0; i < nr_vertices; i++) {
        adj_matrix[i] = (int*)calloc(nr_vertices, sizeof(int));
    }

    int from_vertex, to_vertex, cost;
    while (fscanf(fp, "%d %d %d", &from_vertex, &to_vertex, &cost) == 3) {
        adj_matrix[from_vertex][to_vertex] = cost;
        adj_matrix[to_vertex][from_vertex] = cost; // Assuming undirected graph
    }

    struct_graph->adj_matrix = adj_matrix;
    struct_graph->nr_vertices = nr_vertices;

    fclose(fp);
    return struct_graph;
}

int main() {
    Graph* graph = read_graph_from_file();

    if (graph == NULL) {
        return 1;
    }

    primMST(graph);

    // Free allocated memory for graph
    for (int i = 0; i < graph->nr_vertices; i++) {
        free(graph->adj_matrix[i]);
    }
    free(graph->adj_matrix);
    free(graph);

    return 0;
}
