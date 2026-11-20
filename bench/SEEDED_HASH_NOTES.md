# Reproducing Q5

The experiment calls the team's `SeededHash` and `StructureA::get(key, value, probes)` directly. See the Q5 section of [the report](../REPORT.md) for the proof and results.

## Design

Generate a fixed sequence of distinct 12-byte printable keys with input seed 203. For each of 16 hash seeds (1001 through 1016), start a fresh chained table with 4,096 buckets. Insert progressively longer prefixes to reach nominal loads of 0.1 through 0.9. All seeds see identical keys in the same order. At each load, check the live count, capacity and value of every stored key.

Count each bucket's occupancy using the shared hash helper. A bucket of length k contributes k² to the sum of the full chain lengths seen by its k stored keys. It also contributes k(k+1)/2 comparisons when each key is looked up exactly once. The driver checks the exact identity sum(k²) = 2 × total successful probes − n against the team's lookup counter for every row.

## Build and run

From the repository root, with C++17 and Python with Matplotlib installed, run the following in a POSIX shell.

```sh
c++ -std=c++17 -O2 bench/q5_chain_length.cpp src/structA/StructureA.cpp -o q5
./q5 > results/q5_chain_length.csv
python3 bench/plot_q5.py
```

For Windows, use an x64 Native Tools Command Prompt for Visual Studio.

```bat
cl /nologo /std:c++17 /EHsc /O2 bench\q5_chain_length.cpp src\structA\StructureA.cpp /Fe:q5.exe
q5.exe > results\q5_chain_length.csv
python bench\plot_q5.py
```

The saved run used MSVC 14.50.35717, x64 /O2, on Windows build 26300 with an Intel Core i7-14650HX. These are node counts. The standard library's uniform integer distribution may map the pseudorandom sequence differently on other toolchains.

## CSV columns

| Column | Meaning |
| --- | --- |
| input_seed | Pseudorandom generator seed for the shared key sequence |
| hash_seed | Seed supplied to the team's hash helper |
| buckets | Fixed number of chains |
| keys | Number of distinct stored keys at this load |
| alpha | Actual keys / buckets after integer rounding |
| mean_chain | Full chain length averaged uniformly over stored keys |
| mean_hit_probes | Actual node comparisons averaged uniformly over successful lookups |
| universal_bound | Theoretical reference 1 + (keys − 1) / buckets |
| longest_chain | Maximum bucket occupancy in this particular run |

[Raw observations](../results/q5_chain_length.csv) · [Figure](../results/q5_chain_length.png)
