import pandas
data = pandas.read_csv("parallel.csv", names=["n", "time"])
data = data.astype({"n": int, "time": float})
#data["n_best"] = data["n"].apply(lambda y: y - 0.125)
#data["n_real"] = data["n"].apply(lambda y: y + 0.125)

print(data)

seq = pandas.read_csv("sequential.csv", names=["n", "time"])
seq = seq.astype({"n": int, "time": float})
seq = seq.iloc[0]["time"]

import matplotlib.pyplot as plot
import matplotlib

picture = "./matvec.pdf"

plot.figure(figsize=(3, 2))
plot.axhline(seq, linestyle="dashed", color="black", label=r"$\mathcal{T}_s$")
plot.bar(
        data["n"], data["time"],
        0.25, color="red", label=r"$\mathcal{T}_p(n)$"
)
plot.xlabel(r"Nombre d'unités de calcul $n$")
plot.xticks(range(1, 9))
plot.ylabel(r"Temps d'exécution [s]")
#plot.yticks(range(0, 101, 20))
plot.legend()
plot.tight_layout()
plot.savefig(picture, format="pdf")
plot.close()
