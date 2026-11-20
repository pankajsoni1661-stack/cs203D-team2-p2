# CS203D Project 2 · Hash tables and word counts

The C++ application stores string keys and integer values using either separate chaining (A) or linear probing (B). Both implementations support insertion, updates, lookup, deletion and a count of distinct keys. The word counter uses the same tables to count words in a text file.

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

## Report and measurements

The report on this branch includes the tombstone example for Q3. The measurements snapshot contains the Q5 analysis, benchmark build and plotting commands, raw observations and figures. Use the code from that snapshot to reproduce its measurements. [Main report](REPORT.md) · [Measurement report](https://github.com/pankajsoni1661-stack/cs203D-team2-p2/blob/69e5f7e/REPORT.md#measurements) · [Benchmark instructions](https://github.com/pankajsoni1661-stack/cs203D-team2-p2/blob/69e5f7e/bench/EXPERIMENTS.md) · [Tool disclosures](TOOLS.md)

## Team roles

| Name | Roll number | GitHub username | Responsibility | Question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | ritikgup-g | Separate chaining | Q1 |
| Kartikey Gupta | 250003045 | kartikey357 | Linear probing | Q2 |
| Pankaj Soni | 250004028 | pankajsoni1661-stack | Application and command interface | Q3 |
| Shubham Poddar | 250004032 | subham2352007-eng | Testing and workload generation | Q4 |
| Adwit Tiwari | 250004004 | Adwit-webdev | Benchmarks, timing runs and report figures | Q5 |
