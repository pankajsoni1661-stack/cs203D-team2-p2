# CS203D Project 2 · Hash tables and word counts

The C++ application stores string keys and integer values using either separate chaining (A) or linear probing (B). Both implementations support insertion, updates, lookup, deletion and a count of distinct keys. The word counter uses the same tables to count words in a text file.

## Requirements and setup

Use **x64 Native Tools Command Prompt for Visual Studio** for all Windows examples below. Install Git for Windows and Visual Studio or Build Tools with the **Desktop development with C++** workload (MSVC and Windows SDK). Python 3 and Matplotlib are needed for the graphs. To start from a fresh folder, clone the project and enter it. If you already have a checkout of `main`, open the prompt in its repository root and skip the clone.

```bat
git clone --branch main https://github.com/pankajsoni1661-stack/cs203D-team2-p2.git
cd cs203D-team2-p2
where cl
```

If your existing checkout is on `adwit-work`, you can open the latest main in a sibling folder. Run these commands once from your existing repository root, then continue in the new folder:

```bat
git fetch origin
git worktree add --detach ..\cs203d-main origin/main
cd ..\cs203d-main
```

For plotting, check that Python is available and install Matplotlib into that interpreter once:

```bat
python --version
python -m pip install matplotlib
```

## Build and run on Windows

Open an **x64 Native Tools Command Prompt for Visual Studio** in the repository root. Build with C++17 and run the supplied command fixture.

```bat
cl /nologo /EHsc /std:c++17 /O2 app\main.cpp src\structA\StructureA.cpp /Fe:hash_app.exe
hash_app.exe --structure A < Application_tests\application_commands.in
```

The fixture updates `apple` to −1, checks its count and value, then deletes it and looks it up again. Expected output:

```text
1
-1
NOT_FOUND
```

Use `--structure B` for linear probing. Both tables default to the fixed hash. Add `--seed 42` after the structure to select repeatable seeded hashing.

| Command | Effect | Output |
| --- | --- | --- |
| `I key value` | Insert or update an integer value | None |
| `G key` | Look up a key | Stored value or `NOT_FOUND` |
| `D key` | Delete a key | `NOT_FOUND` if absent, otherwise none |
| `N` | Count distinct live keys | Integer count |

## Word counting and checks

The word counter lowercases ASCII letters, splits at all other characters and prints counts in first-appearance order. Run these commands from the repository root in Command Prompt.

```bat
hash_app.exe --structure B --words Application_tests\application_words.in
hash_app.exe --structure B --seed 42 --compare-words data\corpus.txt
```

The second command checks that fixed and seeded word counts agree. To test command-mode output against a fixture, use a temporary output file and compare it line by line:

```bat
hash_app.exe --structure B --seed 42 < Application_tests\application_resize.in > "%TEMP%\cs203d_resize_actual.txt"
fc Application_tests\application_resize.expected "%TEMP%\cs203d_resize_actual.txt"
```

## Run the correctness tests on main

Build `hash_app.exe` above first. The following commands run the Structure B unit tests and the 20,000-operation invariant checks, then generate a 100,000-operation input and compare each table's output with the sorted-array oracle. Run from the repository root so the generator can write its two files under `tests\`. Each `fc` comparison should report that the files match.

```bat
cl /nologo /EHsc /std:c++17 /O2 /I. tests\test_StructB.cpp src\structA\StructureA.cpp /Fe:unit_b.exe
unit_b.exe
cl /nologo /EHsc /std:c++17 /O2 tests\invariant_checker.cpp src\structA\StructureA.cpp /Fe:invariants.exe
invariants.exe
cl /nologo /EHsc /std:c++17 /O2 tests\differential_test.cpp /Fe:make_cases.exe
make_cases.exe
hash_app.exe --structure A < tests\random_ops.txt > "%TEMP%\cs203d_A.txt"
fc /b tests\oracle_out.txt "%TEMP%\cs203d_A.txt"
hash_app.exe --structure B < tests\random_ops.txt > "%TEMP%\cs203d_B.txt"
fc /b tests\oracle_out.txt "%TEMP%\cs203d_B.txt"
```

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

[Project report](REPORT.md) · [Experiment design](bench/EXPERIMENTS.md) · [Q5 protocol](bench/SEEDED_HASH_NOTES.md) · [Tool disclosures](TOOLS.md)

## Team roles

| Name | Roll number | GitHub username | Responsibility | Question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | ritikgup-g | Separate chaining | Q1 |
| Kartikey Gupta | 250003045 | kartikey357 | Linear probing | Q2 |
| Pankaj Soni | 250004028 | pankajsoni1661-stack | Application and command interface | Q3 |
| Shubham Poddar | 250004032 | subham2352007-eng | Testing and workload generation | Q4 |
| Adwit Tiwari | 250004004 | Adwit-webdev | Benchmarks, timing runs and report figures | Q5 |
