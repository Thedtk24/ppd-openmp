import pandas as pd
import matplotlib.pyplot as plt

# === Lecture du CSV ===
col_names = ["num_threads", "time_kernel", "time_reference"]
df = pd.read_csv("reports/report.csv", names=col_names, header=0)  # header=0 si le CSV contient un en-tête
df.columns = df.columns.str.strip()  # supprimer les espaces éventuels

# Vérification rapide
print("Colonnes lues :", df.columns)
print(df.head())

threads = df["num_threads"]
time_para = df["time_kernel"]
time_seq = df["time_reference"].iloc[0]  # temps séquentiel constant

plt.figure(figsize=(10,6))

# Courbe parallèle en rouge
plt.plot(threads, time_para, marker='o', linestyle='-', color='red', label="Parallèle")

# Ligne séquentielle en bleu pointillé
plt.hlines(time_seq, xmin=threads.min(), xmax=threads.max(),
           colors='blue', linestyles='--', label="Séquentiel")

# === Paramètres du graphique ===
plt.title("Comparaison temps séquentiel vs parallèle selon le nombre de threads")
plt.xlabel("Nombre de threads OpenMP")
plt.ylabel("Temps d'exécution (secondes)")
plt.xticks(range(1, 9))
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
