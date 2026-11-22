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
fig, ax = plt.subplots(figsize=(7, 4))
for structure, marker in (("A", "o"), ("B", "s")):
    means, errors = [], []
    for size in sizes:
        path = results / "replay" / f"attack_{structure}_{size}.csv"
        raw = path.read_bytes()
        encoding = "utf-16" if raw.startswith((b"\xff\xfe", b"\xfe\xff")) else "utf-8-sig"
        records = csv.DictReader(raw.decode(encoding).splitlines())
        rows = [r for r in records if r["hash_mode"] == "fixed"]
        numbered = [r for r in rows if r["run"] != "mean"]
        summary = [r for r in rows if r["run"] == "mean"]
        valid = (len(rows) == 6 and len(summary) == 1
                 and sorted(r["run"] for r in numbered) == list("12345")
                 and all(r["verification"] == "PASS" and r["structure"] == structure
                         and r["seed"] == "NA" for r in rows)
                 and all(int(r["final_keys"]) == size for r in numbered))
        times = [float(r["replay_ms"]) for r in numbered]
        if not valid or not all(isfinite(t) and t > 0 for t in times):
            raise ValueError(f"Invalid replay records in {path}")
        avg = mean(times)
        if abs(avg - float(summary[0]["replay_ms"])) > 0.000002:
            raise ValueError(f"Mean mismatch in {path}")
        means.append(avg)
        errors.append(stdev(times))
    print(structure, "fixed", [round(x, 2) for x in means])
    ax.errorbar(sizes, means, yerr=errors, marker=marker, capsize=4, label=f"Structure {structure}")
ax.set(title="Historical fixed-hash replay", xlabel="Distinct keys", xticks=sizes,
       ylabel="Mean replay time (ms, log scale)", yscale="log")
ax.legend()
fig.tight_layout()
fig.savefig(results / "figs" / "replay_fixed.png", dpi=160)
