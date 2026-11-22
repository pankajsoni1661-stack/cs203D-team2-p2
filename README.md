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
| Adwit Tiwari | 250004004 | `Adwit-webdev` | Measurement owner | Q5 |# CS203D Project 2 · Hash tables and word counts

The C++ application stores string keys and integer values using either separate chaining (A) or linear probing (B). Both implementations support insertion, updates, lookup, deletion and a count of distinct keys. The word counter uses the same tables to count words in a text file.\

<br>

## 2. Requirements

* **Languages:** C++17 (data structures, CLI application, tests, and benchmarks) and Python 3 (graph plotting)
* **C++ Compiler:** `g++` (GCC 9.0+ or MinGW-w64) supporting `-std=c++17` and `-O2`
* **Python Interpreter:** Python 3.8+ with `matplotlib` (`pip install matplotlib`) for generating plots in `results/figs/ 

## # Testing & Verification Commands (`tests/`)

This guide covers building and executing all correctness, invariant, and adversarial collision tests for **Structure A (Separate Chaining)** and **Structure B (Linear Probing)** in both **fixed base-31** and **seeded** hash modes.

---

## 1. Prerequisites & Build Unified Application

Run all commands from the **repository root directory** (`cs203D-team2-p2-main/`).

### Initial Commands (Windows)
The 1st command may not be required it is just for folder setup but try it if not works without it.
```cmd
cd cs203D-team2-p2-main
```
 2
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
# Run the Application in Command Prompt (CMD)

Run these commands from your project folder. Compilation requires g++ to be installed and available in PATH.

## 1. Open your project folder

Replace `YOUR_PROJECT_FOLDER_PATH` with the actual path to the folder containing `app`, `src`, `data` and `tests`.

```cmd
cd /d "YOUR_PROJECT_FOLDER_PATH"
```

## 2. Compile the application

```cmd
g++ -std=c++17 -O2 app/main.cpp src/structA/StructureA.cpp -o hash_app.exe
```

## 3. Run the word frequency counter

Structure A:

```cmd
hash_app.exe --structure A --words data/corpus.txt
```

Structure B:

```cmd
hash_app.exe --structure B --words data/corpus.txt
```

These commands print each word and its frequency.

## 4. Check that fixed and seeded word counts match

```cmd
hash_app.exe --structure A --seed 42 --compare-words data/corpus.txt
hash_app.exe --structure B --seed 42 --compare-words data/corpus.txt
```

## 5. Generate attack inputs once

```cmd
g++ -std=c++17 -O2 tests/collision_generator.cpp -o collision_generator.exe
collision_generator.exe
```

This creates input files containing 1,024, 4,096 and 8,192 colliding keys in the `tests` folder.

## 6. Run attack mode

```cmd
hash_app.exe --structure A --seed 42 --attack tests/collision_8192.txt
hash_app.exe --structure B --seed 42 --attack tests/collision_8192.txt
```

Each attack command runs fixed and seeded hashing five times each, verifies that their answers and final states match, and prints the timings and their means.

Replace `8192` with `1024` or `4096` to run the smaller attack inputs.

## Run Adwit's benchmarks and graphs

**To regenerate the graphs from saved data on main**, use the following four commands. The first reads `results/probes.csv` and `results/corpus.csv`, the second reads `results/q5_chain_length.csv`, the third plots the historical fixed-hash baseline from `results/replay/`, and the fourth plots the final shared-hash measurements from `results/seeded_replay.csv`. The Q5 figure is written to `results/q5_chain_length.png`. The other comparison figures are written under `results/figs/`.

```bat
python bench\plot_measurements.py
python bench\plot_q5.py
python bench\plot_replay.py
python bench\plot_seeded_replay.py
```

**To collect fresh probe, corpus and Q5 measurements**, open an x64 Native Tools Command Prompt for Visual Studio at the repository root. Use Python 3 with Matplotlib for plotting. The commands below build the current drivers and replace the corresponding measurement CSVs.

```bat
cl /nologo /EHsc /std:c++17 /O2 bench\measure.cpp src\structA\StructureA.cpp /Fe:measure.exe
cl /nologo /EHsc /std:c++17 /O2 bench\q5_chain_length.cpp src\structA\StructureA.cpp /Fe:q5.exe
measure.exe probes > results\probes.csv
measure.exe corpus data\corpus.txt > results\corpus.csv
q5.exe > results\q5_chain_length.csv
python bench\plot_measurements.py
python bench\plot_q5.py
python bench\plot_replay.py
python bench\plot_seeded_replay.py
```

From the repository root, use the same Native Tools Command Prompt to run the full test suite and collect fresh probe, corpus, and Q5 measurements. The command also redraws the replay graphs from the saved timings. With GCC, omit `--msvc`.

```bat
python tests/run_suite.py --msvc --scale --bench
```

## How to run the program on a test file

### Build & Run Command
```cmd
g++ -O2 -std=c++17 app/main.cpp src/structA/StructureA.cpp -o run.exe
run.exe --structure A < sample_test.txt
```

### Expected Output
35
NOT_FOUND
2
NOT_FOUND
1
NOT_FOUND


## Known Limitations

1. **String Keys & Signed 32-Bit Integer Values Only:**
   * Both data structures store `std::string` keys and signed 32-bit `int` values (`INT_MAX` ceiling). Keys cannot contain whitespace characters (spaces, tabs, or newlines) because command parsing relies on whitespace-delimited `std::istream >>` extraction, and values exceeding `2,147,483,647` (`INT_MAX`) trigger an overflow guard in word-counting mode.

2. **No Automatic Table Shrinking (`shrinkCapacity`):**
   * Both **Structure A** (`doubleCapacity`) and **Structure B** (`resize`) double their bucket/slot arrays when the load factor exceeds `maxLoad` (`1.0` and `0.7`, respectively), but neither implementation shrinks the table when a large fraction of keys are deleted (`D`). Memory allocated during peak load remains reserved until the table is destroyed.

3. **Seeded hashing cost.** Structures A and B both call `string_hash::seededHash`. Each call initializes `std::mt19937_64` with the table's seed and generates one coefficient per byte of the key. Repeated lookups regenerate those coefficients.

4. **Tombstone Accumulation Under Sustained Churn (Structure B):**
   * In `LinearProbingTable`, heavy cycles of repeated insertions and deletions without net growth accumulate `TOMBSTONE` slots. Although tombstones are counted toward the resize trigger (`(num_keys + num_tombstones + 1) / capacity > 0.7`) and purged upon resizing, unsuccessful `get()` queries still scan across all tombstones in a cluster prior to a resize.

5. **Non-Thread-Safe & Non-Copyable Structures:**
   * Neither structure uses locks or atomic operations, so concurrent access from multiple threads is not supported. Additionally, copy constructors and copy assignment operators are explicitly disabled (`= delete`) in `StructureA` (and shallow-pointer unsafe if copied in `LinearProbingTable`) to prevent double-free memory errors.



