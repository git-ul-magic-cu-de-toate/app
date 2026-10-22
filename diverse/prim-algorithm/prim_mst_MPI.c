#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>
#include <string.h>

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


    int N = 0; // number of vertices
    edge_t **adj = NULL; // adjacency lists
    int *adj_count = NULL; // number of edges for each vertex

    if (rank == MASTER) {
        // Read the graph from file
        FILE *fp = fopen("in_files/graph5.in", "r");
        if (!fp) {
            fprintf(stderr, "Error: could not open file %s\n", "in_files/graph5.in");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(fp, "%d", &N);

        
        int capacity = 1000;
        int edge_count = 0;

        // Temporary edge storage
        typedef struct {int u, v, w;} input_edge_t;
        input_edge_t *edges = malloc(capacity * sizeof(input_edge_t));

        while (1) {
            int u,v,w;
            int ret = fscanf(fp, "%d %d %d", &u, &v, &w);
            if (ret != 3) break;
            // Store edge
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

        // Count edges per vertex
        for (int i = 0; i < edge_count; i++) {
            adj_count[edges[i].u]++;
            adj_count[edges[i].v]++; // undirected
        }

        for (int i = 0; i < N; i++) {
            adj[i] = (edge_t*)malloc(adj_count[i]*sizeof(edge_t));
        }

        // We need a pointer to fill each adjacency list
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

    // Broadcast N
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
            // Pick the unvisited vertex with smallest dist
            int u = -1;
            int min_d = INT_MAX;
            for (int i = 0; i < N; i++) {
                if (!visited[i] && dist[i] < min_d) {
                    min_d = dist[i];
                    u = i;
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

            // Broadcast chosen vertex to all workers
            // If we're done, later we send -1 to signal completion.
            MPI_Bcast(&u, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

            // Now we must distribute adj[u] among workers.
            // We'll send count of edges for u to each process, then edges.
            int total_edges_u = adj_count[u];

            // Determine how many edges each process gets
            int base = total_edges_u / size;
            int remainder = total_edges_u % size;

            // Send the number of edges each rank will receive:
            // MASTER already knows its portion. For workers:
            for (int r = 1; r < size; r++) {
                int count_for_r = base + (r < remainder ? 1 : 0);
                MPI_Send(&count_for_r, 1, MPI_INT, r, 0, MPI_COMM_WORLD);
            }

            // MASTER portion:
            int master_count = base + (0 < remainder ? 1 : 0);

            // Now send the edges to workers
            // We'll iterate over the adjacency and send chunks
            int offset = master_count;
            for (int r = 1; r < size; r++) {
                int count_for_r = base + (r < remainder ? 1 : 0);
                if (count_for_r > 0) {
                    MPI_Send(&adj[u][offset], count_for_r*2, MPI_INT, r, 0, MPI_COMM_WORLD);
                    offset += count_for_r;
                }
            }

            // Now MASTER processes its portion
            for (int i = 0; i < master_count; i++) {
                int nbr = adj[u][i].to;
                int w = adj[u][i].weight;
                if (!visited[nbr] && dist[nbr] > w) {
                    dist[nbr] = w;
                    parent[nbr] = u;
                }
            }

            // Receive updates from workers
            // Each worker will send how many updates it has, then pairs (vertex, dist)
            for (int r = 1; r < size; r++) {
                int updates;
                MPI_Recv(&updates, 1, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int k = 0; k < updates; k++) {
                    int nbr_dist[2];
                    MPI_Recv(&nbr_dist, 2, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    int nbr = nbr_dist[0];
                    int w = nbr_dist[1];
                    if (!visited[nbr] && dist[nbr] > w) {
                        dist[nbr] = w;
                        parent[nbr] = u;
                    }
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
        // They do not store the global graph. They just receive portions from MASTER.
        int done = 0;
        while (!done) {
            int u;
            MPI_Bcast(&u, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
            if (u == -1) {
                // done
                break;
            }

            // Receive number of edges for this worker
            int count_for_me;
            MPI_Recv(&count_for_me, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            edge_t *my_edges = NULL;
            if (count_for_me > 0) {
                my_edges = (edge_t*)malloc(count_for_me * sizeof(edge_t));
                // Receive edges
                int *buf = (int*)malloc(count_for_me*2*sizeof(int));
                MPI_Recv(buf, count_for_me*2, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i = 0; i < count_for_me; i++) {
                    my_edges[i].to = buf[2*i];
                    my_edges[i].weight = buf[2*i+1];
                }
                free(buf);
            }

            
            int updates = count_for_me; 
            MPI_Send(&updates, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
            for (int i = 0; i < count_for_me; i++) {
                int nbr_dist[2] = {my_edges[i].to, my_edges[i].weight};
                MPI_Send(nbr_dist, 2, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
            }

            if (my_edges) free(my_edges);
        }
    }

    MPI_Finalize();
    return 0;
}
