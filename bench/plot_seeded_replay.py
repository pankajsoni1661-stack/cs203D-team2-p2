"""Plot the final-build seeded replay means from the saved observations."""
import csv
from pathlib import Path
from statistics import mean, stdev

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

results = Path(__file__).resolve().parents[1] / "results"
with (results / "seeded_replay.csv").open(newline="") as stream:
    rows = list(csv.DictReader(stream))

for structure in "AB":
    sizes = sorted({int(row["keys"]) for row in rows if row["structure"] == structure})
    means, deviations = [], []
    for size in sizes:
        group = [r for r in rows if r["structure"] == structure and int(r["keys"]) == size]
        if sorted(int(r["run"]) for r in group) != list(range(1, 6)) or any(
            r["verification"] != "PASS" or int(r["final_keys"]) != size for r in group
        ):
            raise ValueError(f"Incomplete or invalid observations for {structure}, {size} keys")
        times = [float(r["replay_ms"]) for r in group]
        means.append(mean(times))
        deviations.append(stdev(times))
    plt.errorbar(sizes, means, yerr=deviations, marker="o", capsize=4, label=structure)
plt.legend(title="Structure")
plt.xlabel("Distinct keys")
plt.ylabel("Mean seeded replay time (ms)")
plt.xticks([1024, 4096, 8192])
plt.tight_layout()
plt.savefig(results / "figs/seeded_replay.png", dpi=150)
