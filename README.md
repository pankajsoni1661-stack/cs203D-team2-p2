## 1. Team and contributions

| Member | Roll no. | GitHub | Role (project spec) | Question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | `ritikgup-g` | Structure A owner | Q1 |
| Kartikey Gupta | 250003045 | `kartikey357` | Structure B owner | Q2 |
| Pankaj Soni | 250004028 | `pankajsoni1661-stack` | Application owner | Q3 |
| Shubham Poddar | 250004032 | `subham2352007-eng` | Test owner (also builds the worst-case inputs, as the spec asks of a five-member team) | Q4 |
| Adwit Tiwari | 250004004 | `Adwit-webdev` | Measurement owner | Q5 |# CS203D Project 2 · Hash tables and word counts

The C++ application stores string keys and integer values using either separate chaining (A) or linear probing (B). Both implementations support insertion, updates, lookup, deletion and a count of distinct keys. The word counter uses the same tables to count words in a text file.

## Requirements

* **Languages:** C++17 (data structures, CLI application, tests, and benchmarks) and Python 3 (graph plotting)
* **C++ Compiler:** `g++` (GCC 9.0+ or MinGW-w64) supporting `-std=c++17` and `-O2`
* **Python Interpreter:** Python 3.8+ with `matplotlib` (`pip install matplotlib`) for generating plots in `results/figs/

## # Testing & Verification Commands (`tests/`)

This guide covers building and executing all correctness, invariant, and adversarial collision tests for **Structure A (Separate Chaining)** and **Structure B (Linear Probing)** in both **fixed base-31** and **seeded** hash modes.

---

## 1. Prerequisites & Build Unified Application

Run all commands from the **repository root directory** (`cs203D-team2-p2-main/`).

### Initial Command
```cmd
cd cs203D-team2-p2-main_final
```
 
### Windows (Command Prompt)
```cmd
g++ -O2 -std=c++17 app/main.cpp src/structA/StructureA.cpp -o run.exe
```


## 2. Internal Invariant Checker (`tests/invariant_checker.cpp`)

Stress-tests **Structure A** and **Structure B** (both fixed and seeded) across **20,000 interleaved operations** (`insert`, `remove`, `get`), verifying internal table invariants every 500 operations.

### Windows (Command Prompt)
```cmd
g++ -O2 -std=c++17 tests/invariant_checker.cpp src/structA/StructureA.cpp -o tests/inv_check.exe
tests\inv_check.exe
```

**Expected Output:**
```text
[PASS] All invariants held for Structure A & B (fixed & seeded) across 20000 operations!
```

---

## 3. Differential Test Against Reference Oracle (`tests/differential_test.cpp` & `tests/oracle.cpp`)

Generates **100,000 randomized operations** (`tests/random_ops.txt`) along with ground-truth outputs (`tests/oracle_out.txt`) using a sorted-array binary-search oracle (`seed = 42`), then verifies zero byte-level discrepancies across all four configurations.

### Windows (Command Prompt)
```cmd
:: Step 1: Compile and generate 100,000 operations + oracle expected output
g++ -O2 -std=c++17 tests/differential_test.cpp -o tests/diff_test.exe
tests\diff_test.exe

:: Step 2: Run Structure A and Structure B (Fixed & Seeded)
run.exe --structure A < tests/random_ops.txt > tests/outA.txt
run.exe --structure A --seed 42 < tests/random_ops.txt > tests/outA_seeded.txt
run.exe --structure B < tests/random_ops.txt > tests/outB.txt
run.exe --structure B --seed 42 < tests/random_ops.txt > tests/outB_seeded.txt

:: Step 3: Compare against Oracle ground truth
fc tests\oracle_out.txt tests\outA.txt
fc tests\oracle_out.txt tests\outA_seeded.txt
fc tests\oracle_out.txt tests\outB.txt
fc tests\oracle_out.txt tests\outB_seeded.txt
```

**Expected Output:**
* Windows: `FC: no differences encountered` for all four files.

---

## 4. Adversarial Collision Attack Generator & Timing Replay (`tests/collision_generator.cpp`)

Generates three worst-case base-31 polynomial hash collision workloads (`1,024`, `4,096`, and `8,192` distinct colliding keys using `"aB"` / ``"`a"`` block substitution) and runs 5-trial timing benchmarks comparing **fixed base-31** vs. **seeded** hashing.

### Windows (Command Prompt)
```cmd
:: Step 1: Generate adversarial collision files
g++ -O2 -std=c++17 tests/collision_generator.cpp -o tests/collision_gen.exe
tests\collision_gen.exe

:: Step 2: Run 5-trial attack timing replay (CSV output)
run.exe --structure A --seed 42 --attack tests/collision_1024.txt
run.exe --structure A --seed 42 --attack tests/collision_4096.txt
run.exe --structure A --seed 42 --attack tests/collision_8192.txt

run.exe --structure B --seed 42 --attack tests/collision_1024.txt
run.exe --structure B --seed 42 --attack tests/collision_4096.txt
run.exe --structure B --seed 42 --attack tests/collision_8192.txt
```
  `
  ## Run Adwit's benchmarks and graphs

**To regenerate the graphs from saved data on main**, use the following three commands. The first reads `results/probes.csv` and `results/corpus.csv`, the second reads `results/q5_chain_length.csv`, and the third validates the six supplied timing files under `results/replay/`. The Q5 figure is written to `results/q5_chain_length.png`. The other comparison figures are written under `results/figs/`.

```bat
python bench\plot_measurements.py
python bench\plot_q5.py
python bench\plot_replay.py
```

**To collect fresh probe, corpus and Q5 measurements**, use the benchmark snapshot below. The C++ drivers on main still use the older shared `SeededHash` interface. Commit `69e5f7e` contains the drivers and matching table implementations used for the report. From the main repository root, create a separate checkout once:

```bat
git worktree add --detach ..\cs203d-measurements 69e5f7e
cd ..\cs203d-measurements
```

Run the following in that measurement checkout. The corpus run reads the included `data/corpus.txt`. The three redirections replace the checkout's saved CSVs with the new runs. Subsequent sessions can start directly in that folder.

```bat
cl /nologo /EHsc /std:c++17 /O2 bench\measure.cpp src\structA\StructureA.cpp /Fe:measure.exe
cl /nologo /EHsc /std:c++17 /O2 bench\q5_chain_length.cpp src\structA\StructureA.cpp /Fe:q5.exe
measure.exe probes > results\probes.csv
measure.exe corpus data\corpus.txt > results\corpus.csv
q5.exe > results\q5_chain_length.csv
python bench\plot_measurements.py
python bench\plot_q5.py
python bench\plot_replay.py
```
