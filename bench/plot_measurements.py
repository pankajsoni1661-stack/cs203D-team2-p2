"""Render all measurement figures from the checked-in raw runs."""
import csv
from collections import defaultdict
from pathlib import Path
from statistics import mean, stdev

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = Path(__file__).resolve().parents[1] / "results"
FIGURES = RESULTS / "figs"
FIGURES.mkdir(exist_ok=True)
BLUE, ORANGE = "#086c89", "#9a5411"
plt.rcParams.update({"font.size": 10, "axes.spines.top": False, "axes.spines.right": False})


def load(name, fields, value):
    groups = defaultdict(list)
    with (RESULTS / f"{name}.csv").open(newline="") as stream:
        for row in csv.DictReader(stream):
            groups[tuple(row[k] for k in fields)].append(float(row[value]))
    if not groups or any(len(values) != 5 for values in groups.values()):
        raise ValueError(f"{name} requires five replicates in each group")
    return groups


def save(fig, name):
    fig.tight_layout()
    fig.savefig(FIGURES / f"{name}.png", dpi=150, facecolor="white")
    plt.close(fig)


def probe_figures():
    groups = load("probes", ["hash", "structure", "search_type", "alpha"], "avg_probes")
    for mode in ("fixed", "seeded"):
        fig, axes = plt.subplots(1, 2, figsize=(10.4, 3.6))
        for ax, table, unit in zip(axes, "AB", ("nodes compared", "slots inspected")):
            for hit, color, symbol in (("successful", BLUE, "o"), ("unsuccessful", ORANGE, "s")):
                points = sorted((float(k[3]), v) for k, v in groups.items() if k[:3] == (mode, table, hit))
                x = [a for a, _ in points]
                theory = [(1+a/2 if hit == "successful" else a) if table == "A" else
                          (1 + 1/(1-a)**(1 if hit == "successful" else 2))/2 for a in x]
                ax.plot(x, theory, "--", color=color, linewidth=1.2)
                ax.errorbar(x, [mean(v) for _, v in points], yerr=[stdev(v) for _, v in points],
                            fmt=symbol, color=color, capsize=3, label=hit)
            ax.plot([], [], "--", color="#454545", label="theory")
            ax.set(title=f"{table}  {'Separate chaining' if table == 'A' else 'Linear probing'}",
                   xlabel="Live keys / capacity", ylabel=f"Mean {unit} per lookup", xticks=[.1,.3,.5,.7,.9])
            ax.legend(frameon=False, fontsize=8)
            ax.grid(axis="y", alpha=.2)
        fig.suptitle(f"Lookup probes with the {mode} hash")
        save(fig, "probes_" + mode)


def timing_figures():
    corpus = load("corpus", ["structure", "hash"], "seconds")
    fig, ax = plt.subplots(figsize=(7.6, 3.4))
    for i, mode in enumerate(("fixed", "seeded")):
        samples = [corpus[st, mode] for st in "AB"]
        ax.bar([j + (i - .5)*.34 for j in range(2)], [mean(v)*1000 for v in samples],
               yerr=[stdev(v)*1000 for v in samples], width=.32, capsize=4,
               label=mode, color=[BLUE, ORANGE][i])
    ax.set(xticks=[0,1], xticklabels=["A   Separate chaining", "B   Linear probing"],
           ylabel="Word-count time (ms)")
    ax.legend(frameon=False)
    save(fig, "corpus_time")


if __name__ == "__main__":
    probe_figures()
    timing_figures()
