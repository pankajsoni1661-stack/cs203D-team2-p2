"""Validate saved replay timings and plot the means with sample standard deviations."""
import csv
from pathlib import Path
from statistics import mean, stdev
from math import isfinite

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

results = Path(__file__).resolve().parents[1] / "results"
sizes = (1024, 4096, 8192)
fig, axes = plt.subplots(1, 2, figsize=(9, 4), sharey=True)
for structure, ax in zip("AB", axes):
    for mode, marker in (("fixed", "o"), ("seeded", "s")):
        means, errors = [], []
        for size in sizes:
            path = results / "replay" / f"attack_{structure}_{size}.csv"
            raw = path.read_bytes()
            encoding = "utf-16" if raw.startswith((b"\xff\xfe", b"\xfe\xff")) else "utf-8-sig"
            rows = list(csv.DictReader(raw.decode(encoding).splitlines()))
            group = [r for r in rows if r["hash_mode"] == mode]
            numbered = [r for r in group if r["run"] != "mean"]
            summary = [r for r in group if r["run"] == "mean"]
            valid = (len(rows) == 12 and len(summary) == 1
                     and sorted(r["run"] for r in numbered) == list("12345")
                     and all(r["verification"] == "PASS" and r["structure"] == structure
                             and r["seed"] == ("42" if mode == "seeded" else "NA") for r in group)
                     and all(int(r["final_keys"]) == size for r in numbered))
            times = [float(r["replay_ms"]) for r in numbered]
            if not valid or not all(isfinite(t) and t > 0 for t in times):
                raise ValueError(f"Invalid replay records in {path}")
            avg = mean(times)
            if abs(avg - float(summary[0]["replay_ms"])) > 0.000002:
                raise ValueError(f"Mean mismatch in {path}")
            means.append(avg)
            errors.append(stdev(times))
        print(structure, mode, [round(x, 2) for x in means])
        ax.errorbar(sizes, means, yerr=errors, marker=marker, capsize=4, label=mode)
    ax.set(title={"A":"A · Separate chaining","B":"B · Linear probing"}[structure],
           xlabel="Distinct keys", xticks=sizes, yscale="log")
    ax.legend()
axes[0].set_ylabel("Mean replay time (ms, log scale)")
fig.tight_layout()
fig.savefig(results / "figs" / "replay_comparison.png", dpi=160)
