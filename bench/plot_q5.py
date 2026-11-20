"""Rebuild the Q5 figure from the per-seed chain measurements."""
import csv
from collections import defaultdict
from pathlib import Path
from statistics import mean, stdev

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

results = Path(__file__).resolve().parents[1] / "results"
by_load = defaultdict(list)
with (results / "q5_chain_length.csv").open(newline="", encoding="utf-8-sig") as stream:
    for row in csv.DictReader(stream):
        by_load[int(row["keys"])].append(row)

loads, measured, deviations, theory = [], [], [], []
for _, rows in sorted(by_load.items()):
    values = [float(row["mean_chain"]) for row in rows]
    loads.append(float(rows[0]["alpha"]))
    measured.append(mean(values))
    deviations.append(stdev(values))
    theory.append(float(rows[0]["universal_bound"]))

fig, ax = plt.subplots(figsize=(8.0, 4.6))
ax.plot(loads, theory, "--", color="#93602e", linewidth=2,
        label="Universal-family reference")
ax.errorbar(loads, measured, yerr=deviations, fmt="o", capsize=4,
            color="#096c81", label="Team hash, mean ± 1 SD across 16 seeds")
ax.set(xlabel="Load factor (live keys / buckets)",
       ylabel="Mean chain length seen by a stored key (nodes)",
       xticks=[i / 10 for i in range(1, 10)], ylim=(1, 2.0))
ax.legend(frameon=False, loc="upper left")
ax.grid(axis="y", alpha=.22)
ax.spines[["top", "right"]].set_visible(False)
fig.tight_layout()
fig.savefig(results / "q5_chain_length.png", dpi=180)
