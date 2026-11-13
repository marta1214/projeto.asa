#!/usr/bin/env python3
"""Benchmark da solucao ASA (empilhamento de cubos).
Uso: python3 bench.py [reps]   (por defeito 10 repeticoes)
Requer: gcc, g++, main.c e gerador.cpp na mesma pasta.
Gera casos com seed fixa, mede o tempo do programa completo (input por
ficheiro, output para /dev/null) e escreve resultados.csv.
"""
import csv, os, statistics, subprocess, sys, time

REPS = int(sys.argv[1]) if len(sys.argv) > 1 else 10
D = 1_000_000                      # max_dimension (fixo)
SEED = 42
TAMANHOS = [100_000 * k for k in range(1, 16)]   # 100k .. 1.5M

subprocess.run(["gcc", "-O2", "-o", "solucao", "main.c"], check=True)
subprocess.run(["g++", "-O2", "-o", "gerador", "gerador.cpp"], check=True)
os.makedirs("casos", exist_ok=True)

linhas = []
for n in TAMANHOS:
    caso = f"casos/n{n}.in"
    if not os.path.exists(caso):
        with open(caso, "w") as f:
            subprocess.run(["./gerador", str(n), str(D), str(SEED)], stdout=f, check=True)
    tempos = []
    for r in range(REPS + 1):                    # 1a corrida = aquecimento (descartada)
        with open(caso) as fin:
            t0 = time.perf_counter()
            subprocess.run(["./solucao"], stdin=fin, stdout=subprocess.DEVNULL, check=True)
            t1 = time.perf_counter()
        if r > 0:
            tempos.append(t1 - t0)
    m, s = statistics.mean(tempos), statistics.stdev(tempos)
    linhas.append((n, m, s))
    print(f"n={n:>9}  media={m:.4f}s  desvio={s:.4f}s", flush=True)

with open("resultados.csv", "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["n", "tempo_medio_s", "desvio_padrao_s"])
    w.writerows(linhas)
print("Escrito resultados.csv")
