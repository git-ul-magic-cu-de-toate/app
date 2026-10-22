#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>
#include <string.h>
#include <omp.h>

#define MASTER 0

typedef struct {
    int to;
    int weight;
} edge_t;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Set the number of OpenMP threads per MPI process
    omp_set_num_threads(4);

    int N = 0; // number of vertices
    edge_t **adj = NULL; // adjacency lists
    int *adj_count = NULL; // number of edges for each vertex

    if (rank == MASTER) {

        FILE *fp = fopen("in_files/graph5.in", "r");
        if (!fp) {
            fprintf(stderr, "Error: could not open file %s\n", "in_files/graph5.in");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(fp, "%d", &N);

        typedef struct {int u, v, w;} input_edge_t;
        int capacity = 1000;
        int edge_count = 0;
        input_edge_t *edges = malloc(capacity * sizeof(input_edge_t));

        while (1) {
            int u,v,w;
            int ret = fscanf(fp, "%d %d %d", &u, &v, &w);
            if (ret != 3) break;
            if (edge_count == capacity) {
                capacity *= 2;
                edges = realloc(edges, capacity * sizeof(input_edge_t));
            }
            edges[edge_count].u = u;
            edges[edge_count].v = v;
            edges[edge_count].w = w;
            edge_count++;
        }
        fclose(fp);

        // Build adjacency lists
        adj = (edge_t**)malloc(N * sizeof(edge_t*));
        adj_count = (int*)calloc(N, sizeof(int));

        for (int i = 0; i < edge_count; i++) {
            adj_count[edges[i].u]++;
            adj_count[edges[i].v]++;
        }

        for (int i = 0; i < N; i++) {
            adj[i] = (edge_t*)malloc(adj_count[i]*sizeof(edge_t));
        }

        int *fill_pos = (int*)calloc(N, sizeof(int));
        for (int i = 0; i < edge_count; i++) {
            int u = edges[i].u;
            int v = edges[i].v;
            int w = edges[i].w;

            adj[u][fill_pos[u]].to = v;
            adj[u][fill_pos[u]].weight = w;
            fill_pos[u]++;

            adj[v][fill_pos[v]].to = u;
            adj[v][fill_pos[v]].weight = w;
            fill_pos[v]++;
        }

        free(fill_pos);
        free(edges);
    }

    // Broadcast N (nr vertices)
    MPI_Bcast(&N, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    int *visited = NULL;
    int *dist = NULL;
    int *parent = NULL;

    if (rank == MASTER) {
        visited = (int*)calloc(N, sizeof(int));
        dist = (int*)malloc(N*sizeof(int));
        parent = (int*)malloc(N*sizeof(int));

        for (int i = 0; i < N; i++) {
            dist[i] = INT_MAX;
            parent[i] = -1;
        }

        // Start Prim from node 0
        dist[0] = 0;

        int mst_count = 0;
        long long cost = 0;

        while (mst_count < N) {
            // Find the unvisited vertex with smallest dist in parallel
            int u = -1;
            int min_d = INT_MAX;

            // Parallel reduction to find min dist vertex
            #pragma omp parallel
            {
                int local_u = -1;
                int local_min = INT_MAX;

                #pragma omp for nowait
                for (int i = 0; i < N; i++) {
                    if (!visited[i] && dist[i] < local_min) {
                        local_min = dist[i];
                        local_u = i;
                    }
                }

                #pragma omp critical
                {
                    if (local_min < min_d) {
                        min_d = local_min;
                        u = local_u;
                    }
                }
            }

            if (u == -1) {
                // No more reachable vertices
                break;
            }

            visited[u] = 1;
            mst_count++;
            if (parent[u] != -1) {
                cost += dist[u];
            }

            // Broadcast chosen vertex
            MPI_Bcast(&u, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

            int total_edges_u = (adj_count == NULL) ? 0 : adj_count[u];

            int base = 0, remainder = 0;
            if (size > 0) {
                base = total_edges_u / size;
                remainder = total_edges_u % size;
            }

            // Send the number of edges to workers
            for (int r = 1; r < size; r++) {
                int count_for_r = base + (r < remainder ? 1 : 0);
                MPI_Send(&count_for_r, 1, MPI_INT, r, 0, MPI_COMM_WORLD);
            }

            int master_count = base + (0 < remainder ? 1 : 0);

            // Distribute edges
            int offset = master_count;
            for (int r = 1; r < size; r++) {
                int count_for_r = base + (r < remainder ? 1 : 0);
                if (count_for_r > 0) {
                    MPI_Send(&adj[u][offset], count_for_r*2, MPI_INT, r, 0, MPI_COMM_WORLD);
                    offset += count_for_r;
                }
            }

            // Master updates dist for its portion of edges in parallel
            #pragma omp parallel for
            for (int i = 0; i < master_count; i++) {
                int nbr = adj[u][i].to;
                int w = adj[u][i].weight;
                if (!visited[nbr] && dist[nbr] > w) {
                    dist[nbr] = w;
                    parent[nbr] = u;
                }
            }

            // Receive updates from workers and apply them in parallel
            // We'll store updates in a buffer first
            for (int r = 1; r < size; r++) {
                int updates;
                MPI_Recv(&updates, 1, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (updates > 0) {
                    int *updates_buffer = (int*)malloc(updates*2*sizeof(int));
                    MPI_Recv(updates_buffer, updates*2, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Apply updates in parallel
                    #pragma omp parallel for
                    for (int k = 0; k < updates; k++) {
                        int nbr = updates_buffer[2*k];
                        int w = updates_buffer[2*k + 1];
                        if (!visited[nbr] && dist[nbr] > w) {
                            #pragma omp critical
                            {
                                if (w < dist[nbr]) {
                                    dist[nbr] = w;
                                    parent[nbr] = u;
                                }
                            }
                        }
                    }
                    free(updates_buffer);
                }
            }

        }

        // Signal completion to workers
        {
            int end_signal = -1;
            MPI_Bcast(&end_signal, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        }

        // Used for testing the corectness of the implementation between different serial and parallel approaches
        // FILE* output_file = fopen("testing_outputs/MPI_mst_output.txt", "w");

        // fprintf(output_file, "Edge Weight\n");
        // for (int v = 1; v < N; v++) {
        //     if (parent[v] != -1) {
        //         fprintf(output_file, "%d %d\n", parent[v], v);
        //     }
        // }

        // fclose(output_file);

        free(parent);
        free(dist);
        free(visited);

        // Free adjacency
        for (int i = 0; i < N; i++) {
            free(adj[i]);
        }
        free(adj);
        free(adj_count);

    } else {
        // Worker processes
        int done = 0;
        while (!done) {
            int u;
            MPI_Bcast(&u, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
            if (u == -1) {
                done = 1;
                break;
            }

            int count_for_me;
            MPI_Recv(&count_for_me, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            edge_t *my_edges = NULL;
            if (count_for_me > 0) {
                my_edges = (edge_t*)malloc(count_for_me * sizeof(edge_t));
                int *buf = (int*)malloc(count_for_me*2*sizeof(int));
                MPI_Recv(buf, count_for_me*2, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i = 0; i < count_for_me; i++) {
                    my_edges[i].to = buf[2*i];
                    my_edges[i].weight = buf[2*i+1];
                }
                free(buf);
            }

            // Workers send all edges as updates

            int updates = count_for_me;
            MPI_Send(&updates, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
            if (updates > 0) {
                int *send_buf = (int*)malloc(updates*2*sizeof(int));
                #pragma omp parallel for
                for (int i = 0; i < updates; i++) {
                    send_buf[2*i] = my_edges[i].to;
                    send_buf[2*i+1] = my_edges[i].weight;
                }
                MPI_Send(send_buf, updates*2, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
                free(send_buf);
            }

            if (my_edges) free(my_edges);
        }
    }

    MPI_Finalize();
    return 0;
}
