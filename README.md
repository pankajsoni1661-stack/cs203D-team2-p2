# CS 203D — Data Structures & Algorithms Mini-Project

* **Project Number:** Project 2
* **Project Name:** Hashing & Collision Resolution (Separate Chaining vs. Linear Probing & Seeded Hashing Defences)
* **Team Name:** Team 2 (`cs203D-team2-p2`)
* **Course:** CS 203D — Data Structures & Algorithms

---

## 1. Team and contributions

| Member | Roll no. | GitHub | Role (project spec) | Question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | `ritikgup-g` | Structure A owner | Q1 |
| Kartikey Gupta | 250003045 | `kartikey357` | Structure B owner | Q2 |
| Pankaj Soni | 250004028 | `pankajsoni1661-stack` | Application owner | Q3 |
| Shubham Poddar | 250004032 | `subham2352007-eng` | Test owner (also builds the worst-case inputs, as the spec asks of a five-member team) | Q4 |
| Adwit Tiwari | 250004004 | `Adwit-webdev` | Measurement owner | Q5 |

The C++ application stores string keys and integer values using either separate chaining (A) or linear probing (B). Both implementations support insertion, updates, lookup, deletion and a count of distinct keys. The word counter uses the same tables to count words in a text file.


## Build and run

From the repository root, use a C++17 compiler, Python 3.9 or newer and Matplotlib. Install the plotting dependency with `python -m pip install matplotlib`. On Windows use an x64 Native Tools Command Prompt for MSVC. On Linux or macOS use a shell with g++ available.

```bat
cl /nologo /EHsc /std:c++17 /O2 app\main.cpp src\structA\StructureA.cpp /Fe:run.exe
run.exe --structure A < Application_tests\application_commands.in
run.exe --structure B --seed 42 --compare-words data\corpus.txt
run.exe --structure A < tests\demo_1000000.in
```

The command fixture prints `1`, `-1`, then `NOT_FOUND` on separate lines. The demo prints `1000000`, `0`, `499999`, and `999999`. Use `--structure B` to repeat either command with linear probing. For GCC build the same application with `g++ -std=c++17 -O2 app/main.cpp src/structA/StructureA.cpp -o run` and invoke `./run`.

## Run the full suite and reproduce figures

```bat
python tests/run_suite.py --msvc --scale --bench
```

With g++ use `python3 tests/run_suite.py --scale --bench`. The suite compiles from current sources in a temporary directory. It runs A and B unit tests, invariants, the command and word fixtures, the standalone oracle, 100,000 random operations and the supplied million-key demonstration. Both structures are checked in fixed and seeded mode. The `--scale` flag adds the 3,501,003-command independent oracle check. The `--bench` flag recollects probe, corpus and Q5 data and redraws their figures and the historical replay comparison. Fresh CSVs replace the corresponding files in `results/`.

To redraw figures directly from the checked-in data:

```bat
python bench/plot_measurements.py
python bench/plot_q5.py
python bench/plot_replay.py
python bench/plot_seeded_replay.py
```

[Measurement commands and definitions](bench/EXPERIMENTS.md) · [Q5 protocol](bench/SEEDED_HASH_NOTES.md) · [Report](REPORT.pdf) · [Slides](docs/slides.pdf) · [Tool disclosures](TOOLS.md)

## Known Limitations

1. **String Keys & Signed 32-Bit Integer Values Only:**
   * Both data structures store `std::string` keys and signed 32-bit `int` values (`INT_MAX` ceiling). Keys cannot contain whitespace characters (spaces, tabs, or newlines) because command parsing relies on whitespace-delimited `std::istream >>` extraction, and values exceeding `2,147,483,647` (`INT_MAX`) trigger an overflow guard in word-counting mode.

2. **No Automatic Table Shrinking (`shrinkCapacity`):**
   * Both **Structure A** (`doubleCapacity`) and **Structure B** (`resize`) double their bucket/slot arrays when the load factor exceeds `maxLoad` (`1.0` and `0.7`, respectively), but neither implementation shrinks the table when a large fraction of keys are deleted (`D`). Memory allocated during peak load remains reserved until the table is destroyed.

3. **Seeded lookup cost.** Both tables regenerate position coefficients from the seed on each hash call.

4. **Tombstones.** Deletion leaves tombstones in B until rebuilding clears them. Tombstones count toward B's resize threshold.
