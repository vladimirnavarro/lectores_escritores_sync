import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import glob

# Read and merge all files
files = sorted(glob.glob("./output/summary_metrics_*.csv"))
df = pd.concat([pd.read_csv(f) for f in files], ignore_index=True)

# Clean and convert columns
df["perf_cpu_cycles"] = pd.to_numeric(df["perf_cpu_cycles"].astype(str).str.extract(r"(\d+)", expand=False), errors='coerce')
df["perf_task_clock_ms"] = pd.to_numeric(df["perf_task_clock_ms"].astype(str).str.extract(r"(\d+\.?\d*)", expand=False), errors='coerce')
df["program_exec_time_sec"] = pd.to_numeric(df["program_exec_time_sec"], errors='coerce')
df["total_throughput_ops_sec"] = pd.to_numeric(df["total_throughput_ops_sec"], errors='coerce')

df["implementation"] = df["implementation"].astype(str)
df["scenario"] = df["scenario"].astype(str)

# Calculate average by implementation and scenario
df_avg = (
    df.groupby(["implementation", "scenario"], as_index=False)
    .agg({
        "program_exec_time_sec": "mean",
        "total_throughput_ops_sec": "mean",
        "perf_cpu_cycles": "mean",
        "perf_task_clock_ms": "mean"
    })
)

# List of metrics
metrics = [
    {
        "columna": "program_exec_time_sec",
        "titulo": "Average Exec Time",
        "ylabel": "Time (s)"
    },
    {
        "columna": "total_throughput_ops_sec",
        "titulo": "Average Total Throughput  ",
        "ylabel": "Ops/sec"
    },
    {
        "columna": "perf_cpu_cycles",
        "titulo": "Average CPU Cycles",
        "ylabel": "Cycles"
    },
    {
        "columna": "perf_task_clock_ms",
        "titulo": "Average Task Clock",
        "ylabel": "CPU Time (ms)"
    }
]

# Get unique implementations
implementations = df_avg["implementation"].unique()

# Create subplots per metric
sns.set(style="whitegrid")
for metric in metrics:
    fig, axes = plt.subplots(1, len(implementations), figsize=(18, 5))  # one row, three columns

    for i, imp in enumerate(implementations):
        df_imp = df_avg[df_avg["implementation"] == imp]
        sns.barplot(
            data=df_imp,
            x="scenario",
            y=metric["columna"],
            ax=axes[i],
            palette="Set2"
        )
        axes[i].set_title(imp)
        axes[i].set_xlabel("Scenario")
        axes[i].set_ylabel(metric["ylabel"])
        axes[i].tick_params(axis='x', rotation=45)

        # Show values above bars
        for container in axes[i].containers:
            axes[i].bar_label(container, fmt="%.2f", label_type="edge", fontsize=9)
    fig.suptitle(metric["titulo"], fontsize=16)

    # Scenario legend in the upper left corner
    legend = "Scenarios:\nR_eq_W = Number of readers equal to writers\nW_gt_R = More writers than readers\nR_gt_W = More readers than writers"
    fig.text(0.02, 0.92, legend, fontsize=14,
             bbox=dict(facecolor='white', edgecolor='gray', boxstyle='round,pad=0.5'),
             ha='left', va='top')

    plt.tight_layout(rect=[0, 0.03, 1, 0.90])
    plt.show()
