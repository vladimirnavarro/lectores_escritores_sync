import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

#Read the CSV file containing the summary metrics
df = pd.read_csv("./output/summary_metrics.csv")

#Convert the columns to numeric types, handling errors and removing units
df["perf_cpu_cycles"] = pd.to_numeric(df["perf_cpu_cycles"].str.split().str[0], errors='coerce')
df["perf_task_clock_ms"] = pd.to_numeric(df["perf_task_clock_ms"].str.split().str[0], errors='coerce')
df["program_exec_time_sec"] = pd.to_numeric(df["program_exec_time_sec"], errors='coerce')
df["total_throughput_ops_sec"] = pd.to_numeric(df["total_throughput_ops_sec"], errors='coerce')

#Convert 'implementation' and 'scenario' columns to string and create a combo column
df["implementation"] = df["implementation"].astype(str)
df["scenario"] = df["scenario"].astype(str)
df["combo"] = df["implementation"] + " - " + df["scenario"]

#Set the style for seaborn plots
sns.set(style="whitegrid")

#Metrics to plot
metricas = [
    {
        "columna": "program_exec_time_sec",
        "titulo": "Program Execution Time por implementación",
        "ylabel": "Tiempo de ejecución (segundos)"
    },
    {
        "columna": "total_throughput_ops_sec",
        "titulo": "Total Throughput por implementación",
        "ylabel": "Throughput total (ops/seg)"
    },
    {
        "columna": "perf_cpu_cycles",
        "titulo": "CPU Cycles usados por implementación",
        "ylabel": "Ciclos de CPU"
    },
    {
        "columna": "perf_task_clock_ms",
        "titulo": "Task Clock Time por implementación",
        "ylabel": "Task Clock (milisegundos)"
    }
]

#Get unique implementations for subplots
implementaciones = df["implementation"].unique()

#Create bar plots for each metric
for metrica in metricas:
    fig, axes = plt.subplots(1, len(implementaciones), figsize=(18, 5))
    
    for i, imp in enumerate(implementaciones):
        df_imp = df[df["implementation"] == imp]
        sns.barplot(data=df_imp, x="scenario", y=metrica["columna"], ax=axes[i], palette="Set2")
        axes[i].set_title(imp)
        axes[i].set_xlabel("Escenario")
        axes[i].set_ylabel(metrica["ylabel"])
        axes[i].tick_params(axis='x', rotation=45)

    fig.suptitle(metrica["titulo"], fontsize=16)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
