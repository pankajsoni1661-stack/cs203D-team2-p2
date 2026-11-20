# Hash Tables and How to Break Them

**CS 203D · Project 2 · Team 2 (`cs203D-team2-p2`)**
Structure A: separate chaining · Structure B: linear probing with tombstones

This report describes the code in `src/`, `app/`, `tests/`, `bench/` and `results/` as supplied in the two project archives (`cs203D-team2-p2-main-final` and `cs203D-team2-p2-main`). Their C++ sources are identical apart from comments. Where a number comes from the team's recorded CSV files it is marked *recorded*. Where it comes from a re-run done while assembling this report it is marked *re-run* and its machine is given. The two kinds are never mixed in one cell.

---

## 1. Team and contributions

| Member | Roll no. | GitHub | Role (project spec) | Question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | `ritikgup-g` | Structure A owner | Q1 |
| Kartikey Gupta | 250003045 | `kartikey357` | Structure B owner | Q2 |
| Pankaj Soni | 250004028 | `pankajsoni1661-stack` | Application owner | Q3 |
| Shubham Poddar | 250004032 | `subham2352007-eng` | Test owner (also builds the worst-case inputs, as the spec asks of a five-member team) | Q4 |
| Adwit Tiwari | 250004004 | `Adwit-webdev` | Measurement owner | Q5 |

The team has five members, so Q6 is not required.

**What each member built**

| Member | Main files | What they contain |
| --- | --- | --- |
| Ritik | `src/structA/StructureA.h`, `StructureA.cpp` | Separate chaining. Fixed base-31 hash, seeded hash, doubling resize, deletion, probe counting, `loadFactor()`, `checkInvariants()`. |
| Kartikey | `src/structB/LinearProbing.hpp`, `Structure_B.cpp`, `tests/test_StructB.cpp` | Linear probing with `EMPTY/OCCUPIED/TOMBSTONE` slots, a configurable `max_load_factor`, resize that clears tombstones, its own seeded hash, five unit tests. |
| Pankaj | `app/main.cpp`, `Application_tests/` | The `--structure A\|B` entry point, the `I/G/D/N` command mode, the word-frequency counter (`--words`, `--compare-words`), the attack replay (`--attack`), four input/expected-output fixtures. |
| Shubham | `tests/oracle.cpp`, `differential_test.cpp`, `invariant_checker.cpp`, `collision_generator.cpp` | Sorted-array oracle, 10⁵-operation differential test, invariant checker for both tables, block-substitution collision files of 1,024 / 4,096 / 8,192 keys. |
| Adwit | `bench/` (`measure.cpp`, `q5_chain_length.cpp`, plotting scripts, `EXPERIMENTS.md`, `SEEDED_HASH_NOTES.md`), `results/` | Probe, corpus and replay measurements and every figure in section 6. |

*Commit links.* The archives contain no `.git` history, so per-member commit links cannot be generated from them. The repository is `github.com/pankajsoni1661-stack/cs203D-team2-p2`; the README links one commit, [`69e5f7e`](https://github.com/pankajsoni1661-stack/cs203D-team2-p2/blob/69e5f7e/REPORT.md#measurements), the measurement snapshot. **[Team: add each member's main commit links here before export.]**

*Tool use* is declared in `TOOLS.md` (see section 9 for what that file still needs).

---

## 2. Problem

A key-value store holds up to one million string keys, with integer values. Keys are printable ASCII without spaces. It must stay fast even if an attacker who knows the hash function sends keys that all collide.

Both structures support the same four operations, and both print identical output on every input:

| Command | Meaning | Output |
| --- | --- | --- |
| `I key value` | insert, or update if the key exists | none |
| `G key` | look up | the value, or `NOT_FOUND` |
| `D key` | delete | `NOT_FOUND` if absent, otherwise none |
| `N` | count stored keys | the number |

The application has three modes. **Command mode** replays an `I/G/D/N` file. The **word-frequency counter** reads a text file, lowercases ASCII letters, splits at every other byte and prints each word's count in first-appearance order. The **attack mode** replays a file of colliding keys against the fixed hash and against a seeded hash, five times each, checking that both give the same answers and the same final contents.

Two hash functions matter. The **fixed** hash is a base-31 polynomial hash that an attacker can predict. The **seeded** hash uses random coefficients chosen from a seed, so a block-substitution attack built for the fixed hash no longer lines up.

---

## 3. Structure A: separate chaining

**Author of this part of the code: Ritik Gupta.**

### 3.1 How it is stored

```
buckets ──► [ 0 ] ─► Node{key,value,next} ─► Node ─► null
            [ 1 ] ─► null
            [ 2 ] ─► Node ─► null
              ⋮
capacity buckets, totalKeys nodes, maxLoad (default 1.0)
```

`buckets` is an array of `Node*` list heads, created with `new Node*[capacity]()` so every head starts as `nullptr`. Each key lives in one heap-allocated `Node{string key; int value; Node* next;}` on the list for bucket `hash(key) % capacity`. The table starts with 16 buckets. Copying is deleted, so two tables cannot free the same nodes.

### 3.2 The two hash functions

| | Fixed | Seeded |
| --- | --- | --- |
| Formula | `h = h·31 + c` over the key's bytes, in 64-bit unsigned arithmetic (so modulo 2⁶⁴) | `h = Σᵢ aᵢ·(cᵢ+1) mod p`, with `p = 1,000,000,007` and one coefficient `aᵢ` per character position |
| Where randomness comes from | none | `mt19937_64(seed)` supplies `a₀, a₁, …`, each uniform in `[0, p−1]` |
| Then | `% capacity` | `% capacity` |

The seeded hash restarts the generator from the same seed on every call, so for a fixed seed every key sees the same coefficients and the function is stable across calls. The price is that every hash call builds a Mersenne-Twister state and draws one number per character. Section 6 shows what that costs.

### 3.3 Operations and complexity

| Operation | How it works | Expected cost | Worst case |
| --- | --- | --- | --- |
| `insert` | Hash, walk the chain; if the key is found, overwrite the value and return. Otherwise resize if needed, then push a new node on the **head** of the chain. | O(1 + α) | O(n) |
| `get` | Hash, walk the chain, count each node visited in `probes`. | O(1 + α) | O(n) |
| `remove` | Hash, walk the chain keeping a `previous` pointer, unlink, `delete` the node. | O(1 + α) | O(n) |
| resize | Allocate `2·capacity` heads, re-hash every node and relink it into the new array. No node is reallocated. | O(n), amortised O(1) per insert | O(n) |
| `count`, `loadFactor`, `bucketCount` | read fields | O(1) | O(1) |
| `checkInvariants` | Visit every node; check it sits in the bucket its hash gives, is found by `get` with the same value, and that the node count equals `totalKeys`. | O(n + Σ chain²) | |

Here α = n / capacity. The probe counter counts **nodes compared**, so an empty bucket costs 0 probes. This definition matters when comparing with textbook formulas in Q1.

### 3.4 Design decisions

1. **Check for the key before resizing.** `insert` walks the chain first and returns early on an update. A resize can therefore only be triggered by a genuinely new key. (Structure B does it the other way round; see section 4.)
2. **Resize with a `while` loop, threshold 1.0.** The test is `(totalKeys + 1) / capacity > maxLoad`. At `maxLoad = 1.0` the table doubles when the next insert would push α past 1, so in steady state α stays between 0.5 and 1. Expected probes are then bounded by constants: at most 1.5 for a hit and at most 1.0 for a miss. Chaining tolerates α near 1 because chains grow linearly with α. The constructor accepts any `maxLoad` in (0, 1] and falls back to 1.0 otherwise, which is how the benchmark can pin α at a chosen value by choosing the initial capacity.
3. **Head insertion.** New nodes go on the front of the chain, which is O(1) with no tail pointer. It also means a successful search finds recently inserted keys first.
4. **Re-hash on resize instead of storing the hash.** `doubleCapacity` calls `hashKey` for every node. Storing each node's full hash would avoid that, at 8 more bytes per node. With the seeded hash, whose cost is high, storing it would have paid off; the team did not take this option.

### 3.5 What A cannot do that B can

A pays for a heap allocation and a pointer per key, and a chain walk is pointer-chasing across memory. B keeps its slots in one contiguous array. A has no equivalent of B's cache locality.

---

## 4. Structure B: linear probing

**Author of this part of the code: Kartikey Gupta.**

### 4.1 How it is stored

`Slot* table` is an array of `capacity` slots, each `{string key; int value; int state}` with state `EMPTY`, `OCCUPIED` or `TOMBSTONE`. The table starts with 16 slots. A key's probe sequence starts at `hash(key) % capacity` and moves one slot at a time, wrapping at the end. The table tracks `num_keys` (live keys) and `num_tombstones`.

| Operation | How it works |
| --- | --- |
| `insert` | Resize first if `(num_keys + num_tombstones + 1) / capacity > max_load_factor` (default **0.7**). Probe from the start slot. Remember the first tombstone seen. Keep scanning past tombstones to rule out an existing copy of the key. On reaching `EMPTY`, store the new key in the remembered tombstone if there was one, otherwise in the empty slot. On finding the key, overwrite the value. |
| `get` | Probe from the start slot. Stop with "not found" only on `EMPTY`. Skip tombstones and non-matching keys. Each slot inspected adds one to `probes`, including the final empty slot of a miss. |
| `remove` | Probe the same way. On finding the key, set its slot to `TOMBSTONE`, decrement `num_keys`, increment `num_tombstones`. |
| resize | Allocate `2·capacity` fresh slots, reset both counters, re-insert every `OCCUPIED` slot of the old table. Tombstones are not copied, so resize clears them. |
| `checkInvariants` | Count `OCCUPIED` slots, check each is found by `get` with the stored value, and that the count equals `num_keys`. |

### 4.2 The two hash functions

The **fixed** hash is the same base-31 polynomial as Structure A, reduced `% capacity`.

The **seeded** hash is different from Structure A's. It keeps a table of random values, one entry for every (character position, byte value) pair, each uniform in `[0, capacity−1]`, drawn from `mt19937_64(seed)`. The hash is the sum of the entries picked by the key's bytes, taken mod `capacity`. The table is regenerated whenever `capacity` changes, so after a resize the function changes, which is consistent because every live key is re-inserted. Each hash is a handful of table lookups, so it is cheap.

> **Note.** The project spec says Structure B uses "the same hash function" as Structure A. That holds for the fixed hash. The seeded hashes of A and B are two different randomised functions (A: position-weighted sum mod 1,000,000,007; B: per-position tabulation mod capacity). Both defend against the attack, but a timing comparison between A-seeded and B-seeded also compares two different hash implementations.

### 4.3 Complexity

Expected cost is O(1) per operation at a bounded load factor, but the constant grows like 1/(1−α)² for a miss (Q2). The worst case is O(n), reached when every key shares one start slot. Resize is O(capacity) and amortised O(1) per insert.

### 4.4 Design decisions

1. **Tombstones count towards load.** The resize test uses `num_keys + num_tombstones`, so a delete-heavy workload cannot let tombstones silently lengthen every probe sequence.
2. **Reuse the first tombstone on insert.** Re-filling a tombstone keeps probe sequences short without breaking the "stop at EMPTY" rule.
3. **Resize always doubles.** It clears tombstones as the spec requires, but it doubles the capacity even when most of the load is tombstones, where re-hashing at the same size would have been enough.
4. **The load check comes before the lookup.** An update to an existing key can trigger a resize. This was checked directly: a table of capacity 8 holding 5 keys grows to 16 when one of those keys is updated, with the key count unchanged. Structure A does not do this. It is a missed optimisation, not a correctness fault.

### 4.5 What B cannot do that A can, and the reverse

* **High load.** B cannot be run near full: at α = 0.9 an unsuccessful search averages 45.6 probes against A's 0.89 (section 6). Its default cap is 0.7. A works at α = 1.
* **Deletion.** A frees a node at once. B leaves a tombstone that costs probe time and counts towards load until the next resize.
* **Memory behaviour.** B keeps all slots in one array and A allocates a node per key (section 3.5). In the 10⁶-key check of section 5 and in the corpus run, B was the faster structure with the fixed hash.
* **API.** B's `get` is not `const` because it updates the `probes` counter, and its capacity is an `int`, so it cannot exceed 2³¹−1 slots. The 10⁶-key run needs 2,097,152 slots, well inside the limit.

---

## 5. Testing

### 5.1 What exists

| Item | Owner | What it does |
| --- | --- | --- |
| **Oracle** `tests/oracle.cpp` | Shubham | A sorted array of key–value pairs with binary search. Insert places the pair at the binary-search position (`vector::insert`, O(n)). Reads `I/G/D/N` and prints the expected answers. |
| **Differential test** `tests/differential_test.cpp` | Shubham | Fixed seed 42 (`mt19937`). Draws a pool of 500 random keys of length 1–25 from `[a-zA-Z0-9]`, then generates **100,000 operations**: about 35% `I`, 35% `D`, 25% `G`, 5% `N`. Writes `tests/random_ops.txt` and the oracle's answers `tests/oracle_out.txt`. Each structure's output is compared to the oracle's. |
| **Invariant checker** `tests/invariant_checker.cpp` | Shubham | 20,000 operations (seed 42) on a pool of 300 keys, applied to four tables at once: A fixed, A seeded, B fixed, B seeded. `checkInvariants()` runs on all four every 500 operations and at the end. |
| **Unit tests, B** `tests/test_StructB.cpp` | Kartikey | Five tests: insert and update; search past a tombstone; tombstone slot reuse; resize doubles capacity and clears tombstones; seeded mode with 100 keys. |
| **Application fixtures** `Application_tests/` | Pankaj | `commands`, `edge_cases`, `resize`, `words`, each with an `.in` and an `.expected` file. |
| **Collision workloads** `tests/collision_generator.cpp` | Shubham | Block-substitution files of 2¹⁰, 2¹², 2¹³ keys, one `I` and one `G` per key (Q4 explains the construction). |

### 5.2 Results

All of the following were re-run for this report on Linux (g++ 13.3, `-O2`, C++17). They were built from the supplied sources with `g++ -std=c++17 -O2 app/main.cpp src/structA/StructureA.cpp` for the application.

| Check | Result |
| --- | --- |
| Unit tests for B (5 tests) | all pass |
| Invariant checker, 20,000 ops × 4 tables | all invariants held |
| Application fixtures (`commands`, `edge_cases`, `resize`), A and B, fixed and `--seed 42` | 12 of 12 match their `.expected` files |
| Word-count fixture, A and B | both match |
| Differential test, 100,000 ops (35,080 `I`, 34,716 `D`, 25,268 `G`, 4,936 `N`; 47,680 expected output lines), A and B, each with fixed, `--seed 42` and `--seed 7` | 6 of 6 match the oracle line for line |
| Re-generated `oracle_out.txt` vs the stored copy | identical |
| Team's recorded outputs `outA.txt`, `outA_seeded.txt`, `outB.txt`, `outB_seeded.txt` vs `oracle_out.txt` | all four identical (ignoring line endings) |
| Attack replays, 1,024 / 4,096 / 8,192 keys, 5 runs each, both modes, both structures | every recorded run is `PASS` with the expected final key count (60 of 60); re-run on Linux also `PASS` |
| **Scale check, 10⁶ keys** (see below) | all four modes match |

**Scale check.** The project states a scale of one million keys, but the team's tests top out at a few thousand keys (the differential test uses a 500-key pool, the attack files at most 8,192 keys, the corpus 10,307 distinct words). A script, `tests/scale_test_gen.py` (seed 20261121), generates 3,501,003 commands: 1,000,000 inserts of distinct random printable keys of length 8–16, `N`, 1,000,000 hits in shuffled order, 500,000 deletes, `N`, 1,000,000 lookups of which half are now missing, 1,000 deletes of already-deleted keys, `N`. The expected answers come from a Python `dict`, which the project allows in tests. Structure A and Structure B both pass in fixed and seeded mode. Wall-clock time for the whole run, including parsing and output, on a 2-vCPU Intel Xeon at 2.1 GHz with g++ 13.3 `-O2` (*re-run*):

| | A fixed | A seeded | B fixed | B seeded |
| --- | ---: | ---: | ---: | ---: |
| 3.5 M commands (s) | 3.3 | 8.4 | 3.0 | 3.3 |

This is a single run per cell, so treat it as a correctness result and a rough size guide, not a benchmark.

### 5.3 Gaps and bugs

* **No Structure A unit-test file.** The spec gives the Structure A owner "Structure A and its unit tests". The repository has `tests/test_StructB.cpp` but no `tests/test_StructA.cpp`. Structure A is covered by the differential test, the invariant checker, the fixtures and the scale check, but not by dedicated unit tests for chain positions, resize and update. **[Team: add `tests/test_StructA.cpp`.]**
* **Bugs the tests found.** The archives contain no log of defects caught by the oracle, the differential test or the invariant checker. **[Team: list them here, with the commit that fixed each.]** The spec asks for this.
* **Differential test scale.** With a 500-key pool the tables stay small (A reaches 512 buckets, B 1,024 slots), so resize is exercised but only at small sizes. The scale check above covers the large end.

---

## 6. Measurements

### 6.1 Machine, compiler and what was measured

*Recorded* probe and corpus measurements: MSVC 14.50.35717, C++17, `/O2`, x64 Windows build 26300, Intel Core i7-14650HX. Each plotted point or bar is a mean of five runs, with error bars of one sample standard deviation.
*Recorded* attack replays: machine, compiler and flags were not recorded.
*Re-run* checks in this report: Intel Xeon 2.1 GHz, 2 vCPUs, Ubuntu 24.04, g++ 13.3, `-O2`.

**Probe definitions.** For A, one probe is one node comparison; an empty bucket costs 0. For B, one probe is one slot inspected, including the empty slot that ends an unsuccessful search. For each of five runs and nine target loads (0.1 to 0.9), fresh 4,096-slot tables are built with no deletions, with floor(4,096 × load) distinct random 12-byte printable keys. Every stored key is looked up once, and the same number of absent 13-byte keys is looked up. A and B receive identical keys in both hash modes. B's `max_load_factor` is raised to 0.9 for this benchmark only, so that it does not resize while being filled.

### 6.2 Probes against load factor

The reference formulas use the actual load α = keys / 4,096, so the 0.9 row uses α = 0.8999. Chaining: unsuccessful α, successful 1 + (n−1)/(2m). Linear probing: unsuccessful ½(1 + 1/(1−α)²), successful ½(1 + 1/(1−α)). The derivations are in Q1 and Q2. All cells are five-run means (*recorded*, fixed hash).

| α | Keys | A hit | A hit ref. | A miss | A miss ref. | B hit | B hit ref. | B miss | B miss ref. |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 0.1 | 409 | 1.050 | 1.050 | 0.112 | 0.100 | 1.056 | 1.055 | 1.130 | 1.117 |
| 0.2 | 819 | 1.095 | 1.100 | 0.196 | 0.200 | 1.120 | 1.125 | 1.272 | 1.281 |
| 0.3 | 1,228 | 1.156 | 1.150 | 0.308 | 0.300 | 1.215 | 1.214 | 1.515 | 1.520 |
| 0.4 | 1,638 | 1.201 | 1.200 | 0.399 | 0.400 | 1.348 | 1.333 | 1.915 | 1.888 |
| 0.5 | 2,048 | 1.257 | 1.250 | 0.490 | 0.500 | 1.496 | 1.500 | 2.421 | 2.500 |
| 0.6 | 2,457 | 1.296 | 1.300 | 0.590 | 0.600 | 1.754 | 1.750 | 3.524 | 3.623 |
| 0.7 | 2,867 | 1.350 | 1.350 | 0.696 | 0.700 | 2.153 | 2.166 | 6.115 | 6.054 |
| 0.8 | 3,276 | 1.395 | 1.400 | 0.802 | 0.800 | 2.933 | 2.998 | 12.754 | 12.976 |
| 0.9 | 3,686 | 1.445 | 1.450 | 0.890 | 0.900 | 5.517 | 5.495 | 45.600 | 50.402 |

Seeded hashing gives the same picture (*recorded*, five-run means):

| α | A hit | A miss | B hit | B miss |
| ---: | ---: | ---: | ---: | ---: |
| 0.1 | 1.051 | 0.100 | 1.056 | 1.115 |
| 0.2 | 1.091 | 0.202 | 1.112 | 1.280 |
| 0.3 | 1.143 | 0.300 | 1.195 | 1.497 |
| 0.4 | 1.200 | 0.403 | 1.340 | 1.909 |
| 0.5 | 1.256 | 0.493 | 1.514 | 2.456 |
| 0.6 | 1.296 | 0.611 | 1.744 | 3.750 |
| 0.7 | 1.357 | 0.704 | 2.160 | 5.982 |
| 0.8 | 1.411 | 0.800 | 3.007 | 13.140 |
| 0.9 | 1.446 | 0.906 | 5.481 | 56.424 |

![Mean lookup probes with the fixed polynomial hash, plotted against load factor](results/figs/probes_fixed.png)

![Mean lookup probes with seeded hashing, plotted against load factor](results/figs/probes_seeded.png)

**Comparison with the expected complexity.**

* **Structure A matches the chaining formulas closely.** Successful searches are within 0.6% (fixed hash) and 0.8% (seeded) of 1 + (n−1)/(2m) at every load. Unsuccessful searches are within 0.013 probes of α everywhere; the largest gap is at α = 0.1 (0.112 against 0.100), about two standard errors of a five-run mean and attributed to sampling noise.
* **Structure B matches the linear-probing formulas up to α = 0.8.** Successful searches are within 2.2% at every load, for both hashes. Unsuccessful searches are within 3.6% for α ≤ 0.8, for both hashes.
* **At α = 0.9 the unsuccessful B search diverges** from the 50.4 reference: 45.6 with the fixed hash (9.5% low) and 56.4 with the seeded hash (12% high), with run-to-run standard deviations of 5.9 and 18.6 probes. Q2 explains why the estimate is least reliable exactly here.
* **The two structures separate sharply with load.** At α = 0.5, B needs 1.50 and 2.42 probes where A needs 1.26 and 0.49. At α = 0.9 a B miss costs about 50 times what an A miss costs.

### 6.3 Corpus word-count timing

The team corpus (Emma, Persuasion, Sense and Sensibility; 2,026,382 bytes) contains 366,832 word tokens and 10,307 distinct words under the application's tokenisation; both figures were re-counted independently for this report and match. The timer covers the lookups, count updates and any resizing, on fresh 16-slot tables. Tokenisation and validation run outside the timer. Every run matched independently accumulated counts. Final α is 0.629 in both structures (10,307 live keys in 16,384 slots).

*Recorded* (MSVC, i7-14650HX), mean ± sample standard deviation over five runs, in milliseconds:

| Structure | Fixed | Seeded |
| --- | ---: | ---: |
| A | 24.37 ± 3.01 | 1,088.54 ± 73.65 |
| B | 19.36 ± 1.86 | 1,065.77 ± 70.41 |

![Mean time in milliseconds to count the corpus in each table and hash mode](results/figs/corpus_time.png)

**Important provenance note on the seeded-B figure.** The benchmark driver that produced `probes.csv` and `corpus.csv` passes one shared `SeededHash` object to both tables (`StructureA a(hash, …)` and `LinearProbingTable b(…, hash)`). That driver does not compile against the final sources, where each structure has its own seeded hash (the README points to commit `69e5f7e` for the snapshot that built it). The B-seeded corpus figure of 1,065.77 ms therefore measures B with the *old, shared, expensive* seeded hash, not the final B of section 4.2.

To check this, the driver was ported to the final API (seeds, key generation and CSV format unchanged) and re-run on the final sources. Two things came out:

* The ported driver reproduces the team's **fixed-hash probe rows exactly** (all 36 cells, 9 loads × 2 structures × hit/miss), every **A-seeded probe row exactly**, and the **Q5 CSV byte for byte**. So the recorded probe data for A and the Q5 data describe the final Structure A faithfully.
* The corpus timings with the final sources (*re-run*, Linux g++ 13.3 `-O2`, five runs, mean ± SD in ms):

| Structure | Fixed | Seeded |
| --- | ---: | ---: |
| A | 12.10 ± 0.28 | 632.06 ± 11.24 |
| B | 11.50 ± 0.02 | 17.54 ± 1.10 |

Absolute times differ from the recorded ones because the machine and compiler differ. The shape is what matters. With the final sources, **A's seeded hash makes the corpus run about 52 times slower** (632 vs 12.1 ms) because `StructureA::seededHash` builds an `mt19937_64` on every call, while **the final B's seeded hash costs only about 1.5 times the fixed one** (17.5 vs 11.5 ms). The sentence "seeded hashing took more time in both structures" is true of the recorded data but is not a property of Structure B as finally written. The B-seeded probe means also move a little with the final B (at α = 0.9: 5.97 hits and 54.5 misses, against 5.48 and 56.4 recorded), within the run-to-run spread. **[Team: ideally re-run `measure corpus` with the ported driver on the Windows machine and replace the B-seeded figure.]**

### 6.4 Running time under attack

Each workload has one insert and one lookup per distinct key. The replay timer covers command parsing, table operations and building the in-memory answers. Seeded runs use seed 42. The table gives mean milliseconds of five numbered runs (*recorded*; machine not recorded). All 60 runs were `PASS`.

| Distinct keys | Commands | A fixed | A seeded | B fixed | B seeded |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 1,024 | 2,048 | 21.59 | 18.65 | 28.58 | 8.52 |
| 4,096 | 8,192 | 241.15 | 79.54 | 355.08 | 33.35 |
| 8,192 | 16,384 | 957.43 | 159.42 | 1,345.13 | 60.04 |

![Fixed and seeded mean replay times for both structures, with sample standard deviations](results/figs/replay_comparison.png)

*Re-run* on the Linux machine, same attack files, five runs, mean in milliseconds:

| Distinct keys | A fixed | A seeded | B fixed | B seeded |
| ---: | ---: | ---: | ---: | ---: |
| 1,024 | 3.42 | 3.83 | 8.97 | 0.79 |
| 4,096 | 44.25 | 15.50 | 137.28 | 3.28 |
| 8,192 | 181.12 | 36.02 | 545.62 | 6.81 |

**What this shows against the expected complexity.**

* **The fixed hash is quadratic.** Doubling the key count from 4,096 to 8,192 multiplies fixed-hash time by 3.97 (A) and 3.79 (B) in the recorded data, and by 4.09 (A) and 3.97 (B) in the re-run. The expected factor for Θ(n²) is 4.
* **The seeded hash is close to linear.** The same doubling multiplies seeded time by 2.00 (A) and 1.80 (B) recorded, 2.32 (A) and 2.08 (B) re-run. The expected factor for Θ(n) is 2.
* **Why the fixed hash is quadratic.** All 2^t keys share one 64-bit hash. In A they land in one chain; in B they form one cluster. Insert number *j* walks about *j*−1 existing entries to rule out a duplicate, and each lookup walks a similar distance. For A at 8,192 keys the inserts make about n(n−1)/2 = 33.6 M node visits and the lookups about n(n+1)/2 = 33.6 M more, total n² ≈ 67.1 M. At 957 ms that is roughly 14 ns per node visit, a plausible figure for pointer-chasing plus a 16-byte string compare. This is back-of-envelope arithmetic, not a profile.
* **Attack against the defence.** At 8,192 keys the fixed replay took 6.0 times as long as the seeded replay for chaining and 22.4 times as long for linear probing (recorded). The re-run gives 5.0 and 80. The ratios differ between machines and builds; the direction and the order of magnitude do not.
* **B suffers more than A under attack.** B-fixed is slower than A-fixed (1,345 vs 957 ms recorded; 546 vs 181 ms re-run). One cluster of colliding keys forces every probe to scan the cluster; B also re-inserts all keys, with the same long scans, at each resize, while A's resize only relinks nodes.
* **A small-size crossover.** At 1,024 keys the recorded A-fixed and A-seeded times are close (21.6 vs 18.7 ms) because the seeded hash's per-call cost has not yet been outweighed by the quadratic penalty.

The `bench/plot_replay.py` script validates the numbered runs, checks the recorded means and regenerates the figure. An earlier set of seeded measurements, `results/seeded_replay.csv`, shows A and B nearly equal (54.3 and 49.4 ms at 8,192 keys). It is consistent with both structures using the shared seeded hash at that time and is not comparable with the final-code replay above. It is kept archived and is not used in the conclusions.

### 6.5 Reproduce

From the repository root, in an x64 Native Tools Command Prompt for Visual Studio (Windows) with Python and Matplotlib:

```bat
cl /nologo /std:c++17 /EHsc /O2 bench\measure.cpp src\structA\StructureA.cpp /Fe:measure.exe
measure.exe probes > results\probes.csv
measure.exe corpus data\corpus.txt > results\corpus.csv
python bench\plot_measurements.py
```

With GCC or Clang: `c++ -std=c++17 -O2 bench/measure.cpp src/structA/StructureA.cpp -o measure`. For this to build against the final sources, the benchmark drivers `bench/measure.cpp` and `bench/q5_chain_length.cpp` need the ported versions that accompany this report (see Appendix A).

---

## 7. Conclusion

**Which structure for this application?** For a store that may hold a million keys, may see many deletions and must stay fast when an adversary knows the hash function, we would choose **Structure A (separate chaining) with a seeded hash**, fixing the cost of its seeded hash first.

* **Predictable cost.** A's probe counts follow α and 1 + α/2 and stay below 1.5 for any load it can reach, so its behaviour at 10⁶ keys is not a surprise. B's miss cost follows ½(1 + 1/(1−α)²) and is 45–56 probes at α = 0.9; it is only safe because the table is resized at 0.7.
* **Deletion is simple and exact.** A unlinks and frees a node. B accumulates tombstones, and the only clean-up is a resize that always doubles.
* **Both structures need the randomised hash, not a different layout.** Under attack both degrade quadratically with the fixed hash (A 957 ms, B 1,345 ms at 8,192 keys) and both recover with the seeded hash. The seeded hash is the actual defence.

**What would change the choice.** B is the faster structure in our data when the hash is cheap: it won the fixed-hash corpus run (19.4 vs 24.4 ms recorded; 11.5 vs 12.1 ms re-run), it won the 10⁶-key check with the fixed hash (3.0 vs 3.3 s), and with its final seeded hash it is much faster than A's on every seeded benchmark run on the final sources (attack replay, corpus re-run, 10⁶-key check). Much of that last gap is not structural: A's seeded hash rebuilds a Mersenne-Twister per call. If A cached its coefficients per position (they depend only on the position and the seed), the comparison would be much closer. If memory locality matters more than worst-case robustness and the load factor is held at or below 0.7, B is a reasonable choice.

**Honest limitations of this work**

1. Q1, Q2 and Q4 are drafted from the repository data (section 8). Their owners must read and be able to defend them in the viva.
2. The seeded-B probe and corpus numbers in `results/` come from the old shared hash (section 6.3).
3. The attack replay machine and build were not recorded; the re-run confirms the shape, not the absolute times.
4. There is no Structure A unit-test file and no bug log (section 5.3).
5. Seeded hashes of A and B differ, although the spec says "the same hash function" (section 4.2).
6. The README builds `hash_app.exe`; the spec's examiner command is `./run --structure A|B`. The `-final` archive also contains prebuilt binaries (`run.exe`, `tests/*.exe`), which do not belong in a source repository.
7. Statistics: five runs per cell, as the spec requires, is enough for the means but not for tail behaviour. The B-miss estimate at α = 0.9 has a standard error of 2.6 (fixed) and 8.3 (seeded) probes.

---

## 8. Answers to Q1–Q5

Q6 is the sixth member's question and is not required for a five-member team.

### Q1. Expected probes under separate chaining, compared with measurement

**Author: Ritik Gupta** (Structure A owner)

**Setting.** There are n keys in m buckets and α = n/m. Assume simple uniform hashing: each key is equally likely to go to any bucket, independently of the others. (The seeded hash gives this on average over seeds; the fixed hash approximates it on random keys. Section 6.2 tests this.) Following the code, a probe is one node comparison, so an empty bucket costs 0.

**Unsuccessful search.** The searched key is not stored. Its bucket is uniform, so the chain it walks has length L with

E[L] = Σᵢ Pr[key i is in that bucket] = n · (1/m) = **α**.

The search compares every node, so E[probes] = α.

**Successful search.** The searched key x is uniformly one of the n stored keys. The probes spent are 1 (for x itself) plus the number of nodes ahead of x in its chain. The code inserts at the head, so the nodes ahead of x are exactly the keys inserted *after* x that fell into the same bucket. For the key inserted *i*-th, n−i keys come after it and each lands in its bucket with probability 1/m, so

E[probes | key i] = 1 + (n − i)/m.

Averaging over i = 1…n:

E[probes] = 1 + (1/n) Σᵢ (n−i)/m = 1 + (n−1)/(2m) = **1 + α/2 − α/(2n)**.

(Inserting at the tail instead gives i−1 in place of n−i and the same average.)

**Consequence of the resize rule.** Chaining doubles when the next insert would push α past 1, so in steady state α ∈ [0.5, 1]. That bounds the expected cost at 1.5 probes for a hit and 1.0 for a miss, independent of n. Each operation also costs O(length of key) to hash.

**Under attack.** If all n keys share one hash, the chain has length n, so a miss costs n probes and a hit about (n+1)/2. The formulas above no longer apply, because the "uniform hashing" assumption has failed. Section 6.4 shows the resulting quadratic replay time.

**Comparison with measurement** (fixed hash, five-run means, 4,096 buckets; full table in section 6.2):

| α | Hit measured | Hit formula | Miss measured | Miss formula |
| ---: | ---: | ---: | ---: | ---: |
| 0.1 | 1.050 | 1.050 | 0.112 | 0.100 |
| 0.3 | 1.156 | 1.150 | 0.308 | 0.300 |
| 0.5 | 1.257 | 1.250 | 0.490 | 0.500 |
| 0.7 | 1.350 | 1.350 | 0.696 | 0.700 |
| 0.9 | 1.445 | 1.450 | 0.890 | 0.900 |

Successful searches agree within 0.6% across all nine loads. Unsuccessful searches agree within 0.013 probes. The largest relative gap is at α = 0.1 (0.112 against 0.100): only about 41 node visits are expected per 409 queries, so a handful of extra visits moves the ratio, and the gap is about two standard errors of a five-run mean. The seeded hash matches equally well (hit 1.446 and miss 0.906 at α = 0.9). The measurements therefore support both formulas.

### Q2. Linear probing: the standard formulas against measurement

**Author: Kartikey Gupta** (Structure B owner)

**The formulas.** For linear probing at load α under uniform hashing, with a probe counting every slot inspected including the empty slot that ends a miss (the same counting the code uses):

* successful search: ½ (1 + 1/(1 − α))
* unsuccessful search: ½ (1 + 1/(1 − α)²)

**Comparison** (five-run means, 4,096 slots, reference computed at the actual α):

| α | Hit, fixed | Hit, seeded | Hit ref. | Miss, fixed | Miss, seeded | Miss ref. |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 0.1 | 1.056 | 1.056 | 1.055 | 1.130 | 1.115 | 1.117 |
| 0.3 | 1.215 | 1.195 | 1.214 | 1.515 | 1.497 | 1.520 |
| 0.5 | 1.496 | 1.514 | 1.500 | 2.421 | 2.456 | 2.500 |
| 0.7 | 2.153 | 2.160 | 2.166 | 6.115 | 5.982 | 6.054 |
| 0.8 | 2.933 | 3.007 | 2.998 | 12.754 | 13.140 | 12.976 |
| 0.9 | 5.517 | 5.481 | 5.495 | 45.600 | 56.424 | 50.402 |

Successful searches match within 2.2% at every load and for both hashes. Unsuccessful searches match within 3.6% for α ≤ 0.8. At α = 0.9 they diverge: the fixed-hash mean is 9.5% below the formula and the seeded mean 12% above.

**Why the formulas and the measurements part ways at high load.**

1. **The estimate gets very noisy.** An unsuccessful search runs to the end of a cluster, and clusters merge, so long clusters are both more likely to be hit and more costly when hit. The spread of the per-run mean therefore grows much faster than the mean. At α = 0.9 the five runs have a standard deviation of 5.9 (fixed) and 18.6 (seeded) probes, so the standard error of the five-run mean is 2.6 and 8.3. The reference 50.4 lies 1.8 standard errors above the fixed mean and 0.7 below the seeded mean. The two hash modes fall on opposite sides, which points to noise and not to a systematic bias. With five runs we cannot say more.
2. **The formulas are asymptotic.** They are the limit m → ∞ at fixed α. Our table has m = 4,096. The finite-size correction grows as α approaches 1, because clusters then reach a significant fraction of the table. We did not measure at other table sizes, so we cannot separate this effect from the noise in point 1.
3. **Uniform hashing is an idealisation.** The formulas assume every key's start slot is uniform and independent of the others. Real hash functions only approximate that. The seeded hash of the final Structure B guarantees only pairwise behaviour, and linear probing is known to be sensitive to hash quality in ways pairwise independence does not control. We have no experiment that isolates this effect.
4. **The hit formula holds better than the miss formula** because a successful search is averaged over all n stored keys, which smooths the cluster-length variation that dominates a miss.

**Reading the result.** Below α = 0.8 the textbook formulas predict the table to within a few percent. At α = 0.9 the formula says 50 probes per miss and the table behaves in line with that within the noise. This is why the default cap in the code is 0.7, where the formulas give 2.17 probes for a hit and 6.05 for a miss, about eight times cheaper per miss than at 0.9. Compared with chaining at the same load, B's miss cost is 1.1 against A's 0.1 at α = 0.1 and 45.6 against 0.89 at α = 0.9.

### Q3. Why does linear probing need tombstones?

**Author: Pankaj Soni** (Application owner)

Suppose two different keys, apple and mango, both start at slot 2. Apple gets slot 2. Mango sees slot 2 is occupied, so it moves to slot 3.

| State | Slot 2 | Slot 3 |
|---|---|---|
| Before deletion | apple | mango |
| Incorrect deletion | EMPTY | mango |
| Deletion with a tombstone | TOMBSTONE | mango |

A search for mango starts at slot 2. If deleting apple marks slot 2 EMPTY, the search stops there and wrongly reports that mango is missing. If slot 2 is a TOMBSTONE, the search continues to slot 3 and finds mango. In Structure B, `remove()` sets the slot to TOMBSTONE, and `get()` only stops early when it sees EMPTY.

*Same situation with the team's own keys (added when assembling the report).* With 16 slots and the base-31 hash, the keys `a`, `q` and `A` have byte values 97, 113 and 65, which are all ≡ 1 (mod 16), so all three start at slot 1. They are stored at slots 1, 2 and 3. This is the situation in `tests/test_StructB.cpp`, Test 2: `remove("q")` makes slot 2 a tombstone, and `get("A")` still succeeds because it passes through that tombstone to slot 3. Had `remove` written EMPTY into slot 2, `get("A")` would have stopped at slot 2 and returned `NOT_FOUND` for a key that is still stored.

### Q4. Block substitution: equal-length blocks with equal hash give 2ᵗ colliding keys

**Author: Shubham Poddar** (Test owner)

**The hash.** For a string s = s₀s₁…s_{L−1} the code computes `h = h·31 + c` byte by byte in 64-bit unsigned arithmetic, so with B = 31 and all arithmetic mod 2⁶⁴,

H(s) = ((s₀·B + s₁)·B + …)·B + s_{L−1} = Σᵢ sᵢ · B^{L−1−i}.

Appending a string w to a prefix whose hash is x gives the hash x · B^{|w|} + H(w).

**Claim.** Let u and v be strings of equal length ℓ with H(u) = H(v). Then for any strings p and q, H(p·u·q) = H(p·v·q).

**Proof.** Let |q| = b. Apply the appending rule twice. Hashing p then u gives H(p)·B^ℓ + H(u). Appending q gives

H(p·u·q) = (H(p)·B^ℓ + H(u))·B^b + H(q) = H(p)·B^{ℓ+b} + H(u)·B^b + H(q).

The same expression holds for v with H(v) in place of H(u). The first and last terms do not depend on the middle block, and H(v)·B^b = H(u)·B^b because H(u) = H(v). So the two hashes are equal. ∎

The proof uses equal length: the exponent ℓ appears in the first term, so blocks of different lengths would change the multiplier of the prefix.

**Corollary: t blocks give 2ᵗ colliding keys.** Take a colliding pair (u, v) of length ℓ and form S(c) = c₁c₂…c_t for every choice vector c ∈ {u, v}ᵗ. Change one block at a time from v to u. Each change is a swap inside a context p·□·q, so by the claim it keeps the hash. After at most t swaps S(c) has become uᵗ, so every S(c) has the hash of uᵗ. They are pairwise distinct, because two choice vectors that differ at block i give strings that differ within the ℓ characters of block i, and the blocks have a fixed width. So there are **2ᵗ distinct keys with one common 64-bit hash**. A common 64-bit hash means a common `hash % capacity` for every capacity, so growing the table never separates them. In A they all land in one chain; in B they share one start slot and form one cluster.

**Finding the pair for base 31.** For two-character blocks (x, y) and (x′, y′) the hashes are 31x + y and 31x′ + y′, equal when 31(x − x′) = y′ − y. Taking x′ = x − 1 needs y′ = y + 31. With x = `a` (97), y = `B` (66): x′ = `` ` `` (96), y′ = `a` (97), and

31·97 + 66 = 3,073 = 31·96 + 97.

So the blocks `aB` and `` `a `` collide, and they are the pair used in `tests/collision_generator.cpp`. This was checked by running the hash on all 1,024 keys for t = 10: the 1,024 keys are distinct and have exactly one distinct hash value. For t = 2 the four keys ``aBaB``, ``aB`a``, `` `aaB `` and `` `a`a `` all hash to 3,073 · (31² + 1) = 2,956,226. The classical pair `Aa` / `BB` (both 2,112) works the same way.

**The sizes used.** t = 10, 12, 13 give the 1,024, 4,096 and 8,192 keys of the three attack files. The same fixed function is used by both structures, so the one set of files attacks A and B alike. Section 6.4 shows the effect.

**Why the seeded hash escapes it.** Under the seeded hash of Structure A, swapping `aB` for `` `a `` at positions i, i+1 changes the sum by aᵢ·(97−96) + aᵢ₊₁·(66−97) = aᵢ − 31·aᵢ₊₁ (mod p). That is zero only if the random coefficients happen to satisfy aᵢ ≡ 31·aᵢ₊₁, which has probability 1/p. The hash is a position-weighted sum with no fixed Horner base, so the identity the proof relied on, equal H(u) and H(v) shifted by B^b, is gone. Q5 gives the general argument.

### Q5. How does a random seed affect the chain containing a stored key?

**Author: Adwit Tiwari** (Measurement owner)

Fix a set of n distinct keys and choose one stored key x. Its chain already contains x. Every other key y adds one more node exactly when y and x land in the same bucket. A universal family chooses a hash function at random such that, for each fixed pair of distinct keys, the probability of a shared bucket is at most 1/m, where m is the number of buckets. Summing the n − 1 collision probabilities gives

**E[length of x's chain] ≤ 1 + (n − 1)/m = 1 + α − 1/m**, where α = n/m.

The expectation is over the hash choice. Linearity of expectation lets us add these probabilities even when the collision events depend on each other. If each pair collides with probability exactly 1/m, the bound is an equality. At half load, a stored key therefore sees about 1.5 nodes in its full chain under that model. A lookup can stop before reaching the end of the chain.

#### What the team's code does

The fixed hash uses base 31. The team's seeded hash restarts a pseudorandom generator from the same seed on every call, producing one repeatable coefficient per character position. It multiplies each coefficient by the corresponding unsigned byte plus one, sums modulo 1,000,000,007, and Structure A reduces the result modulo the bucket count. All keys in a table use the same coefficients. A different seed changes how the original characters contribute, so two strings that shared a fixed hash can move apart. Applying a seed only to their already equal hash values would give equal outputs again.

The bound above assumes a pairwise collision guarantee for the **final bucket index**, with the input chosen independently of the hash. Pseudorandom coefficients and a final reduction modulo m warrant an empirical check. Knowing the seed and then choosing the inputs falls outside that assumption.

#### What the implementation measured

We held the input sequence fixed and varied only the hash seed at each load. With 4,096 buckets and 16 seeds, we looked up every stored key at each of nine loads and verified its value. A bucket of length k is seen by k stored keys, so the measured mean full chain length is the sum of k² over buckets divided by n. We checked this against the table's actual successful lookup counts at every load.

| Nominal load | Distinct keys | Universal-family reference | Measured full chain (nodes) | Successful lookup (nodes compared) |
| --- | ---: | ---: | ---: | ---: |
| 0.1 | 409 | 1.100 | 1.108 | 1.054 |
| 0.5 | 2,048 | 1.500 | 1.509 | 1.255 |
| 0.9 | 3,686 | 1.900 | 1.903 | 1.452 |

![Measured chain length over 16 seeds compared with the universal-family reference](results/q5_chain_length.png)

At the highest load, the complete chain averaged 1.903 nodes and a successful lookup examined 1.452. The difference comes from stopping when the requested key is found. The finite-seed averages tracked the reference across this input set. An expectation bound allows individual samples and finite averages on either side of the reference. A guarantee for every fixed pair would require a separate proof for the implemented family.

[Measurement protocol and build commands](bench/SEEDED_HASH_NOTES.md) · [Raw runs](results/q5_chain_length.csv) · [Team implementation](src/structA/StructureA.cpp)

*Check added when assembling the report.* Porting `q5_chain_length.cpp` to the final `StructureA` API and re-running it reproduces `results/q5_chain_length.csv` byte for byte, so the table above describes the final Structure A.

---

## 9. References

**Course and reading list**

1. J. L. Carter and M. N. Wegman, "Universal classes of hash functions," *Journal of Computer and System Sciences* 18(2):143–154, 1979.
2. S. A. Crosby and D. S. Wallach, "Denial of service via algorithmic complexity attacks," *Proceedings of the 12th USENIX Security Symposium*, 2003.
3. T. H. Cormen, C. E. Leiserson, R. L. Rivest and C. Stein, *Introduction to Algorithms*, 3rd ed., MIT Press, 2009, Chapter 11 (hash tables; chaining analysis used in Q1).
4. R. Sedgewick and K. Wayne, *Algorithms*, 4th ed., Addison-Wesley, 2011, §3.4 (hash tables).
5. D. E. Knuth, *The Art of Computer Programming*, Vol. 3, *Sorting and Searching*, 2nd ed., Addison-Wesley, 1998, §6.4 (the linear-probing formulas used in Q2).
6. J. Aspnes, *Randomization*, notes, Yale University (section on universal hashing), `https://www.cs.yale.edu/homes/aspnes/pinewiki/C%282f%29Randomization.html` (cited in Q5).
7. CS 203D / CS 253 course lecture notes on hashing (S. Mazumdar), as listed in `TOOLS.md`.
8. CS 203D End Term Project specification, "Mini Projects", Project 2.

**Tools**

* Microsoft Visual C++ 14.50.35717 (the team's recorded measurements); g++ 13.3 on Ubuntu 24.04 (the re-runs in this report).
* Python 3 and Matplotlib (figures and CSV checks).
* `std::mt19937`, `std::mt19937_64` and `std::uniform_int_distribution` from the C++ standard library.

**AI tools.** As declared in `TOOLS.md`: Google Gemini (Shubham, test design and documentation); a Claude conversation and Codex (Adwit, Q5 and the benchmark drivers). **`TOOLS.md` must also declare** that this assembled report, including the drafts of Q1, Q2 and Q4, the section 5 and 6 checks, the ported benchmark drivers and `tests/scale_test_gen.py`, was produced with Claude from the repository contents; and what each owner then changed. The spec allows declared use but penalises an author who cannot explain their own text in the viva.

---

## Appendix A. Files and commands that accompany this report

| File | Purpose |
| --- | --- |
| `tests/scale_test_gen.py` | Generates the 10⁶-key scale test and its expected output (seed 20261121). `python3 tests/scale_test_gen.py`, then `./run --structure A < scale_1m.in \| cmp - scale_1m.expected` and the same for `--structure B`, with and without `--seed 42`. |
| `bench/measure.cpp` | The original driver ported to the final API (`StructureA(bool useSeeded, seed, initialCapacity)` and `LinearProbingTable(init, 31, useSeeded, seed)`). Reproduces the recorded fixed-hash probe rows exactly. |
| `bench/q5_chain_length.cpp` | The Q5 driver ported to the final API. It keeps a private copy of A's seeded hash to count bucket occupancy, and its built-in identity check Σk² = 2·(total probes) − n fails if that copy ever disagrees with the real hash. Reproduces `results/q5_chain_length.csv` exactly. |

Build and run the whole test set on Linux or macOS from the repository root:

```sh
g++ -std=c++17 -O2 -o run app/main.cpp src/structA/StructureA.cpp
g++ -std=c++17 -O2 -o difftest tests/differential_test.cpp && ./difftest   # writes tests/random_ops.txt, oracle_out.txt
g++ -std=c++17 -O2 -o oracle tests/oracle.cpp
g++ -std=c++17 -O2 -o invcheck tests/invariant_checker.cpp src/structA/StructureA.cpp && ./invcheck
g++ -std=c++17 -O2 -I. -o test_B tests/test_StructB.cpp && ./test_B
for s in A B; do ./run --structure $s < tests/random_ops.txt | cmp - tests/oracle_out.txt && echo "$s ok"; done
```
