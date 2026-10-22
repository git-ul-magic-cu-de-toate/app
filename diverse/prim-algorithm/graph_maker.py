import matplotlib.pyplot as plt

# Function to plot bar charts
def bar_chart(categories, values, title, xlabel, ylabel, save_name):
    plt.figure(figsize=(8, 6))
    plt.bar(categories, values, color='skyblue', edgecolor='black')

    plt.title(title, fontsize=16, weight='bold')
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)

    plt.grid(axis='y', linestyle='--', linewidth=0.5)

    plt.tight_layout()
    plt.savefig(f"plotted_figs/{save_name}.png", dpi=300, bbox_inches='tight')
    plt.close()

# Function to plot line charts
def line_chart(x_values, all_run_times, title, xlabel, ylabel, save_name, legend=None, colors = None):
    plt.figure(figsize=(8, 6))

    for i in range(len(all_run_times)):
        plt.plot(x_values, all_run_times[i], marker='o', linestyle='-', label=legend[i], color=colors[i])

        plt.title(title, fontsize=16, weight='bold')
        plt.xlabel(xlabel, fontsize=12)
        plt.ylabel(ylabel, fontsize=12)

        if legend[i]:
            plt.legend(fontsize=8)

    plt.grid(True, which='both', linestyle='--', linewidth=0.5)

    plt.tight_layout()
    plt.savefig(f"plotted_figs/{save_name}.png", dpi=300, bbox_inches='tight')
    plt.close()

# Data for plotting
# graphs_nr_vertices = [5000, 20000, 100000, 100000, 150000, 20000]
graphs_mean_nr_links = [4, 40, 12, 4, 30, 800]

# Calculated input sizes
# input_sizes = [b / a for a, b in zip(graphs_nr_vertices, graphs_mean_nr_links)]
serial_IPC = [1.422, 1.480, 1.447, 1.479, 1.489, 1.833]

# Bar chart for IPC scores
graphs = ['graph1', 'graph2', 'graph3', 'graph4', 'graph5', 'graph6']
values_IPC_serial = [1.422, 1.480, 1.447, 1.479, 1.489, 1.833]
bar_chart(graphs, values_IPC_serial, "IPC Scores", "Graphs", "IPC", "serial_IPC")

# Bar chart for execution times
times_serial = [0.244, 4.732, 73.727, 78.045, 178.396, 27.328]
bar_chart(graphs, times_serial, "Serial Execution Time", "Graphs", "Time (s)", "serial_times")

# ================================ openMP ================================

times_parallel_openMP_16cores = [0.231, 1.642, 15.548, 13.883, 40.202, 16.771]
times_parallel_openMP_8cores = [0.199, 1.762, 18.860, 18.431, 46.868, 19.177]
times_parallel_openMP_4cores = [0.170, 1.893, 29.107, 26.487, 65.828, 19.998]
times_parallel_openMP_2cores = [0.189, 2.442, 47.858, 43.532, 117.709, 23.607]

times_parallel_openMP = [times_serial, times_parallel_openMP_16cores, times_parallel_openMP_8cores, \
                         times_parallel_openMP_4cores, times_parallel_openMP_2cores]

legend = ["serial", "16 threads", "8 threads", "4 threads", "2 threads"]
colors = ["black", "skyblue", "orange", "green", "red", "purple", "brown"]

# Line chart for parallel openMP execution
line_chart(
    graphs,
    times_parallel_openMP,
    "Parallel OpenMP execution times",
    "Input graphs",
    "Time(s)",
    "openMP_times",
    legend,
    colors
)

times_parallel_openMP_input5 = [186.252, 117.709, 85.537, 65.828, 61.919, 55.246, 50.243, 46.868, \
                                51.657, 41.468, 40.467, 44.819, 43.402, 40.666, 42.117, 40.202]

threads = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]

legend = ["Graph5 - 150000 nodes"]
colors = ["skyblue"]

line_chart(
    threads,
    [times_parallel_openMP_input5],
    "OpenMP Graph5 execution times",
    "Nr. Threads",
    "Time(s)",
    "openMP_times_graph5",
    legend,
    colors
)


speedup_times_openMP = [times_serial[4] / t_parallel_openMP for t_parallel_openMP in times_parallel_openMP_input5]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [speedup_times_openMP],
    "Speedup OpenMP-Serial",
    "Nr. Threads",
    "",
    "openMP_speedup",
    legend,
    colors
)


efficiency_openMP = [speedup / nr_threads for speedup, nr_threads in zip(speedup_times_openMP, threads)]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [efficiency_openMP],
    "Efficiency OpenMP",
    "Nr. Threads",
    "",
    "openMP_efficiency",
    legend,
    colors
)

# ================================ Different scheduling ================================

static_schedule_times_2threads = [104.129, 103.745, 130.856, 109.792, 111.736, 135.518, 112.429, \
                                    113.367, 112.098, 113.936]
static_schedule_times_4threads = [85.947, 85.040, 73.560, 68.054, 69.224, 73.846, 69.025, 65.785, \
                                    68.012, 65.833]     
static_schedule_times_8threads = [117.673, 140.842, 134.340, 110.935, 69.749, 67.673, 48.540, 51.517, \
                                    47.869, 51.455]
static_schedule_times_16threads = [93.251, 143.275, 120.775, 96.528, 48.680, 42.753, 38.318, 36.502, \
                                    39.112, 42.128]

dynamic_schedule_times_2threads = [537.301, 248.848, 102.978, 102.477, 102.612, 116.022, 106.268, 104.167, \
                                    102.082, 116.186]
dynamic_schedule_times_4threads = [610.518, 550.415, 480.152, 278.424, 184.243, 132.523, 87.625, 78.280, \
                                    66.548, 72.974]
dynamic_schedule_times_8threads = [842.412, 653.214, 572.498, 465.184, 252.421, 180.742, 77.424, 72.192, \
                                    67.421, 75.215]
dynamic_schedule_times_16threads = [882.831, 798.482, 594.490, 491.245, 382.124, 285.175, 104.523, 92.491, \
                                    76.136, 82.415]

chunks = [2, 4, 16, 64, 256, 512, 1024, 2048, 4096, 8192]

schedule_static = [static_schedule_times_2threads, static_schedule_times_4threads, \
                    static_schedule_times_8threads, static_schedule_times_16threads]

schedule_dynamic = [dynamic_schedule_times_2threads, dynamic_schedule_times_4threads, \
                    dynamic_schedule_times_8threads, dynamic_schedule_times_16threads]

legend = ["2 Threads", "4 Threads", "8 Threads", "16 Threads"]
colors = ["skyblue", "orange", "green", "purple"]

line_chart(
    chunks,
    schedule_static,
    "Static Schedule Run",
    "Chunk size",
    "Time(s)",
    "openMP_static_schedule",
    legend,
    colors
)

line_chart(
    chunks,
    schedule_dynamic,
    "Dynamic Schedule Run",
    "Chunk size",
    "Time(s)",
    "openMP_dynamic_schedule",
    legend,
    colors
)


# ================================ Varying the paralelization grade ================================

times_without_parallel_minKey_function = [182.601, 160.240, 155.711, 152.025, 165.195, 148.822, 159.962, \
                    144.238, 155.394, 151.940, 160.313, 161.295, 161.010, 157.244, 153.139, 142.924]

times_without_parallel_primMST_function = [184.698, 134.482, 110.658, 117.247, 95.635, 118.237, 106.578, \
                    108.016, 121.535, 108.475, 111.238, 105.886, 108.732, 105.689, 102.725, 107.208]

times_compare_parallelization = [times_parallel_openMP_input5, times_without_parallel_minKey_function, \
                                 times_without_parallel_primMST_function]

legend = ["full parallel", "without parallel minKey", "without parallel primMST"]
colors = ["skyblue", "orange", "green"]

line_chart(
    threads,
    times_compare_parallelization,
    "Grades of Parallelization",
    "Nr. Threads",
    "Time(s)",
    "openMP_grade_parallelization",
    legend,
    colors
)

# ================================ Pthreads ================================

times_parallel_pthreads_16cores = [2.102, 11.026, 85.418, 85.570, 190.252, 43.115]
times_parallel_pthreads_8cores = [1.095, 6.202, 69.880, 69.613, 154.704, 34.791]
times_parallel_pthreads_4cores = [0.589, 4.224, 61.698, 59.826, 138.154, 28.165]
times_parallel_pthreads_2cores = [0.485, 4.075, 85.942, 66.437, 170.322, 31.172]

times_parallel_pthreads = [times_serial, times_parallel_pthreads_16cores, times_parallel_pthreads_8cores, \
                         times_parallel_pthreads_4cores, times_parallel_pthreads_2cores]

legend = ["serial", "16 threads", "8 threads", "4 threads", "2 threads"]
colors = ["black", "skyblue", "orange", "green", "red", "purple", "brown"]

line_chart(
    graphs,
    times_parallel_pthreads,
    "Parallel Pthreads execution times",
    "Input graphs",
    "Time(s)",
    "pthread_times",
    legend,
    colors
)

times_parallel_pthreads_input5 = [195.140, 170.322, 150.671, 138.154, 144.520, 160.329, 157.267, 154.704, \
                                154.701, 155.086, 156.379, 163.698, 156.648, 160.133, 161.087, 190.252]


legend = ["Graph5 - 150000 nodes"]
colors = ["skyblue"]

line_chart(
    threads,
    [times_parallel_pthreads_input5],
    "Pthreads Graph5 execution times",
    "Nr. Threads",
    "Time(s)",
    "pthreads_times_graph5",
    legend,
    colors
)


speedup_times_pthreads = [times_serial[4] / t_parallel_pthreads for t_parallel_pthreads in times_parallel_pthreads_input5]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [speedup_times_pthreads],
    "Speedup Pthread-Serial",
    "Nr. Threads",
    "",
    "pthread_speedup",
    legend,
    colors
)


efficiency_pthreads = [speedup / nr_threads for speedup, nr_threads in zip(speedup_times_pthreads, threads)]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [efficiency_pthreads],
    "Efficiency Pthread",
    "Nr. Threads",
    "",
    "pthread_efficiency",
    legend,
    colors
)



# ================================ MPI ================================
# De decomentat cand se ruleaza MPI pentru aflarea timpilor de executie.

times_parallel_MPI_16cores = [2.507, 5.084, 52.871, 54.113, 107.933, 27.435]
times_parallel_MPI_8cores = [2.104, 4.625, 58.023, 57.595, 126.876, 31.187]
times_parallel_MPI_4cores = [1.929, 4.008, 50.211, 49.417, 108.240, 26.410]
times_parallel_MPI_2cores = [1.784, 3.828, 49.287, 48.963, 110.572, 23.547]

times_parallel_MPI = [times_serial, times_parallel_MPI_16cores, times_parallel_MPI_8cores, \
                         times_parallel_MPI_4cores, times_parallel_MPI_2cores]

legend = ["serial", "16 threads", "8 threads", "4 threads", "2 threads"]
colors = ["black", "skyblue", "orange", "green", "red", "purple", "brown"]

line_chart(
    graphs,
    times_parallel_MPI,
    "Parallel MPI execution times",
    "Input graphs",
    "Time(s)",
    "MPI_times",
    legend,
    colors
)

times_parallel_MPI_input5 = [124.913, 110.572, 109.245, 108.240, 111.245, 118.271, 121.891, 126.876, \
                                115.193, 119.012, 113.791, 121.998, 110.591, 109.071, 110.123, 107.933]


legend = ["Graph5 - 150000 nodes"]
colors = ["skyblue"]

line_chart(
    threads,
    [times_parallel_MPI_input5],
    "MPI Graph5 execution times",
    "Nr. workers",
    "Time(s)",
    "MPI_times_graph5",
    legend,
    colors
)


speedup_times_MPI = [times_serial[4] / t_parallel_pthreads for t_parallel_pthreads in times_parallel_MPI_input5]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [speedup_times_MPI],
    "Speedup MPI-Serial",
    "Nr. workers",
    "",
    "MPI_speedup",
    legend,
    colors
)


efficiency_MPI = [speedup / nr_threads for speedup, nr_threads in zip(speedup_times_MPI, threads)]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [efficiency_MPI],
    "Efficiency MPI",
    "Nr. workers",
    "",
    "MPI_efficiency",
    legend,
    colors
)



# ================================ Hybrid ================================

times_parallel_hybrid_16cores = [3.683, 7.347, 104.659, 118.189, 205.285, 36.137]
times_parallel_hybrid_8cores = [3.746, 4.433, 39.410, 33.993, 108.477, 18.827]
times_parallel_hybrid_4cores = [4.335, 4.806, 23.290, 23.051, 44.664, 25.019]
times_parallel_hybrid_2cores = [1.928, 3.328, 37.663, 37.396, 80.007, 18.831]

times_parallel_hybrid = [times_serial, times_parallel_hybrid_16cores, times_parallel_hybrid_8cores, \
                         times_parallel_hybrid_4cores, times_parallel_hybrid_2cores]

legend = ["serial", "16 threads", "8 threads", "4 threads", "2 threads"]
colors = ["black", "skyblue", "orange", "green", "red", "purple", "brown"]

line_chart(
    graphs,
    times_parallel_hybrid,
    "Parallel hybrid execution times",
    "Input graphs",
    "Time(s)",
    "Hybrid_times",
    legend,
    colors
)

times_parallel_hybrid_input5 = [118.189, 80.007, 59.421, 44.664, 58.422, 63.692, 78.635, 108.477, \
                                117.451, 128.245, 132.412, 148.451, 161.518, 185.532, 191.007, 205.285]


legend = ["Graph5 - 150000 nodes"]
colors = ["skyblue"]

line_chart(
    threads,
    [times_parallel_hybrid_input5],
    "Hybrid Graph5 execution times",
    "Nr. workers",
    "Time(s)",
    "Hybrid_times_graph5",
    legend,
    colors
)


speedup_times_hybrid = [times_serial[4] / t_parallel_pthreads for t_parallel_pthreads in times_parallel_hybrid_input5]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [speedup_times_hybrid],
    "Speedup Hybrid-Serial",
    "Nr. workers",
    "",
    "Hybrid_speedup",
    legend,
    colors
)


efficiency_hybrid = [speedup / nr_threads for speedup, nr_threads in zip(speedup_times_hybrid, threads)]

legend = ["Graph5 - 150000 nodes"]
colors = ["blue"]

line_chart(
    threads,
    [efficiency_hybrid],
    "Efficiency Hybrid",
    "Nr. workers",
    "",
    "Hybrid_efficiency",
    legend,
    colors
)
