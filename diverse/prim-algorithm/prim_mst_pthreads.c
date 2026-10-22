#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int** adj_matrix;
    int nr_vertices;
} Graph;

typedef struct {
    int* key;
    int* mstSet;
    int nr_vertices;
    int start;
    int end;
    int* local_min;
    int* local_min_index;
    pthread_mutex_t* mutex;
} ThreadData;

// Global variables
int global_min = INT_MAX;
int global_min_index = -1;
pthread_mutex_t global_mutex;

// Function to find the minimum key value (pthreads version)
void* minKeyThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int local_min = INT_MAX, local_min_index = -1;

    for (int v = data->start; v < data->end; v++) {
        if (data->mstSet[v] == 0 && data->key[v] < local_min) {
            local_min = data->key[v];
            local_min_index = v;
        }
    }

    pthread_mutex_lock(data->mutex);
    if (local_min < *data->local_min) {
        *data->local_min = local_min;
        *data->local_min_index = local_min_index;
    }
    pthread_mutex_unlock(data->mutex);

    return NULL;
}

int minKey(int key[], int mstSet[], int nr_vertices, int num_threads) {
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int chunk_size = (nr_vertices + num_threads - 1) / num_threads;

    int local_min = INT_MAX;
    int local_min_index = -1;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < num_threads; i++) {
        thread_data[i] = (ThreadData){
            .key = key,
            .mstSet = mstSet,
            .nr_vertices = nr_vertices,
            .start = i * chunk_size,
            .end = (i + 1) * chunk_size > nr_vertices ? nr_vertices : (i + 1) * chunk_size,
            .local_min = &local_min,
            .local_min_index = &local_min_index,
            .mutex = &mutex,
        };

        pthread_create(&threads[i], NULL, minKeyThread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return local_min_index;
}

// Prim's algorithm
void primMST(Graph* graph, int num_threads) {
    int* parent = (int*)malloc(graph->nr_vertices * sizeof(int));
    int* key = (int*)malloc(graph->nr_vertices * sizeof(int));
    int* mstSet = (int*)malloc(graph->nr_vertices * sizeof(int));

    for (int i = 0; i < graph->nr_vertices; i++) {
        key[i] = INT_MAX;
        mstSet[i] = 0;
    }

    key[0] = 0;
    parent[0] = -1;

    for (int count = 0; count < graph->nr_vertices - 1; count++) {
        int u = minKey(key, mstSet, graph->nr_vertices, num_threads);

        mstSet[u] = 1;

        for (int v = 0; v < graph->nr_vertices; v++) {
            if (graph->adj_matrix[u][v] && mstSet[v] == 0 && graph->adj_matrix[u][v] < key[v]) {
                key[v] = graph->adj_matrix[u][v];
                parent[v] = u;
            }
        }
    }

    // Used for testing the corectness of the implementation between different serial and parallel approaches
    // FILE* output_file = fopen("testing_outputs/pthreads_mst_output.txt", "w");
    // if (output_file == NULL) {
    //     perror("Error opening output file");
    //     free(parent);
    //     free(key);
    //     free(mstSet);
    //     return;
    // }

    // fprintf(output_file, "Edge Weight\n");
    // for (int i = 1; i < graph->nr_vertices; i++) {
    //     fprintf(output_file, "%d - %d %d\n", parent[i], i, graph->adj_matrix[i][parent[i]]);
    // }

    // fclose(out_file);

    free(parent);
    free(key);
    free(mstSet);
}

// Read graph from file
Graph* read_graph_from_file() {
    Graph* struct_graph = (Graph*)malloc(sizeof(Graph));

    const char* in_file = "in_files/graph5.in";
    FILE* fp = fopen(in_file, "r");

    if (fp == NULL) {
        printf("Could not open file %s\n", in_file);
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
        adj_matrix[to_vertex][from_vertex] = cost;
    }

    struct_graph->adj_matrix = adj_matrix;
    struct_graph->nr_vertices = nr_vertices;

    fclose(fp);
    return struct_graph;
}

int main() {
    int num_threads = 16;
    Graph* graph = read_graph_from_file();
    if (graph == NULL) {
        return -1;
    }

    pthread_mutex_init(&global_mutex, NULL);
    primMST(graph, num_threads);
    pthread_mutex_destroy(&global_mutex);

    for (int i = 0; i < graph->nr_vertices; i++) {
        free(graph->adj_matrix[i]);
    }
    free(graph->adj_matrix);
    free(graph);

    return 0;
}