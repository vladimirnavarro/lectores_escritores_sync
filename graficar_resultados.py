import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Leer el CSV
df = pd.read_csv("resultados.csv")

# Crear columna "config" (Lectores-Escritores)
df["config"] = df["readers"].astype(str) + "-" + df["writers"].astype(str)

# Obtener las versiones únicas
versiones = df["version"].unique()

# Estilo general
sns.set(style="whitegrid")

# Crear panel de subgráficas SIN compartir eje Y
fig, axes = plt.subplots(1, 3, figsize=(18, 5))  # quitar sharey=True

for i, version in enumerate(versiones):
    df_filtrado = df[df["version"] == version]
    sns.lineplot(ax=axes[i], data=df_filtrado, x="config", y="time", marker="o", color="royalblue")

    axes[i].set_title(f"{version}", fontsize=13)
    axes[i].set_xlabel("Lectores - Escritores", fontsize=11)
    axes[i].set_ylabel("Tiempo (seg)", fontsize=11)
    axes[i].tick_params(axis='x', rotation=45)

# Título general del panel
fig.suptitle("Tiempo de ejecución por configuración - Cada versión con su propia escala", fontsize=15)

plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
