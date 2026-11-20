# Probe and corpus measurements

Build from the repository root in an x64 Native Tools Command Prompt for Visual Studio.

```bat
cl /nologo /std:c++17 /EHsc /O2 bench\measure.cpp src\structA\StructureA.cpp /Fe:measure.exe
measure.exe probes > results\probes.csv
measure.exe corpus data\corpus.txt > results\corpus.csv
```

With GCC or Clang, build with `c++ -std=c++17 -O2 bench/measure.cpp src/structA/StructureA.cpp -o measure` and run `./measure` with the same arguments. Each mode writes CSV to standard output and returns a nonzero status on a verification failure.

## Search probes

For each of five runs and nine target loads (0.1 through 0.9), build fresh 4,096-slot tables without deletions. Insert floor(4,096 × target load) distinct 12-byte printable keys. Query each stored key once and the same number of 13-byte absent keys. For each run and load, A and B receive identical inputs in fixed and seeded modes. The input generator seed is 31000 + 10 × run + load step, counting both from one. The hash seed is 1000 + run in seeded mode.

A counts nodes compared. B counts slots inspected, including the empty slot ending a miss. Only the probe benchmark sets B's max_load_factor to 0.9. The driver verifies the actual capacity and live count before measuring, then checks the answer to every lookup. Each CSV row records the actual load and average probes for one run, table, hash mode and search type.

## Corpus timing

Use the team's data/corpus.txt (Emma, Persuasion, and Sense and Sensibility). Match the application's tokenization by lowercasing ASCII letters and splitting at every other byte. Starting from fresh 16-slot tables with the normal resize settings, time only the word-count lookups and updates, including any resizing they trigger. Run all four table/hash combinations five times, with seed 5000 + run in seeded mode. Outside the timer, compare every word frequency and the distinct count against independently accumulated counts. The CSV records total tokens as keys, distinct words as live, and elapsed wall-clock time in seconds.
