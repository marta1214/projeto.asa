#!/usr/bin/env python3
"""Le resultados.csv e gera grafico1.png (tempo vs n) e grafico2.png (tempo vs n*log2(n))."""
import csv, math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

n, m, s = [], [], []
with open("resultados.csv") as f:
    for r in csv.DictReader(f):
        n.append(int(r["n"])); m.append(float(r["tempo_medio_s"])); s.append(float(r["desvio_padrao_s"]))
n, m, s = np.array(n), np.array(m), np.array(s)
nlogn = n * np.log2(n)

def grafico(x, xlabel, ficheiro, fit=False):
    fig, ax = plt.subplots(figsize=(6, 4.5))
    ax.errorbar(x, m, yerr=s, fmt="o-", color="tab:green", ecolor="tab:blue",
                capsize=3, markersize=4, linewidth=1)
    if fit:
        a, b = np.polyfit(x, m, 1)
        ax.plot(x, a * x + b, "--", color="gray", linewidth=0.8, label="regressão linear")
        r2 = np.corrcoef(x, m)[0, 1] ** 2
        ax.legend(title=f"R² = {r2:.4f}", loc="upper left")
        print(f"{ficheiro}: R2 = {r2:.5f}")
    ax.set_xlabel(xlabel); ax.set_ylabel("Tempo (s)")
    ax.set_xlim(left=0); ax.set_ylim(bottom=0)
    ax.grid(alpha=0.3)
    fig.tight_layout(); fig.savefig(ficheiro, dpi=200); plt.close(fig)

grafico(n, "n (número de blocos)", "grafico1.png")
grafico(nlogn, "n · log₂(n)", "grafico2.png", fit=True)
# para comparar: R2 do ajuste linear a n
print("R2 (tempo vs n) = %.5f" % (np.corrcoef(n, m)[0, 1] ** 2))
