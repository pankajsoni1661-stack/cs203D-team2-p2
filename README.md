# CS203D Project 2 — Hash tables and how to break them

**Team 2** · `cs203D-team2-p2`

This project compares two hash tables that store string keys and integer values. Structure A uses separate chaining and Structure B uses linear probing. Both use the same command interface and can run the word-frequency counter on the same text corpus.

## Team members

| Member | Roll number | GitHub username | Role | Analysis question |
| --- | --- | --- | --- | --- |
| Ritik Gupta | 250004036 | `ritikgup-g` | Structure A and its unit tests | Q1 — Expected search probes for separate chaining |
| Kartikey Gupta | 250003045 | `kartikey357` | Structure B and its unit tests | Q2 — Linear probing formulas and measurements |
| Pankaj Soni | 250004028 | `pankajsoni1661-stack` | Common interface, application, and input/output | Q3 — Why linear probing needs tombstones |
| Shubham Poddar | 250004032 | `subham2352007-eng` | Oracle, differential and invariant tests, and worst-case inputs | Q4 — Why block substitution preserves the polynomial hash |
| Adwit Tiwari | 250004004 | `Adwit-webdev` | Benchmarks, timings, and figures | Q5 — Universal hashing and expected chain length |

## Requirements and build

- C++11 or newer. The application was tested with MinGW GCC 6.3.0 on Windows.
- Run the commands below in PowerShell from the repository root.
- The build requires both teammates’ implementations under `src/structA/` and `src/structB/`.

```powershell
g++ -std=c++11 -Wall -Wextra app/main.cpp src/structA/StructureA.cpp -o app.exe
```

Structure B is implemented in `LinearProbing.hpp`. Its separate driver, `Structure_B.cpp`, has its own `main()` and is built separately.

## Run the command interface

```powershell
Get-Content tests/application_commands.in | .\app.exe --structure A
Get-Content tests/application_commands.in | .\app.exe --structure B
```

The application reads from standard input. Keys are printable ASCII strings without spaces. Values are integers in the C++ `int` range.

| Command | Result |
| --- | --- |
| `I key value` | Insert or update. No output. |
| `G key` | Print the value or `NOT_FOUND`. |
| `D key` | Delete the key. Print `NOT_FOUND` if absent. |
| `N` | Print the number of stored keys. |

The example in `tests/application_commands.in` is:

```text
I apple 4
I apple -1
N
G apple
D apple
G apple
```

Both structures should produce the output in `tests/application_commands.expected`:

```text
1
-1
NOT_FOUND
```

## Run the word-frequency counter

`data/corpus.txt` combines three Jane Austen novels, *Emma*, *Persuasion*, and *Sense and Sensibility*. The Project Gutenberg editions come from the [NLTK Gutenberg collection](https://github.com/nltk/nltk_data/blob/gh-pages/packages/corpora/gutenberg.zip).

```powershell
.\app.exe --structure A --words data/corpus.txt > counts_A.txt
.\app.exe --structure B --words data/corpus.txt > counts_B.txt
```

Use the same text file for both runs. Each run prints one `word count` line per distinct word in order of first appearance. The counter converts ASCII letters to lowercase and treats other characters as separators. Both structures matched a reference count of **366,832 words and 10,307 distinct words** with the supplied corpus.

## Run the application checks

After building, paste this PowerShell block to check both structures against the expected-output files:

```powershell
& {
    foreach ($structure in 'A', 'B') {
        foreach ($test in 'application_commands', 'application_edge_cases', 'application_resize', 'application_words') {
            $file = "tests/$test.in"
            if ($test -eq 'application_words') {
                $actual = @(.\app.exe --structure $structure --words $file)
            } else {
                $actual = @(Get-Content -LiteralPath $file | .\app.exe --structure $structure)
            }
            $exitCode = $LASTEXITCODE
            $expected = @(Get-Content -LiteralPath "tests/$test.expected")
            if ($exitCode -ne 0 -or ($actual -join "`n") -cne ($expected -join "`n")) {
                throw "FAIL: Structure $structure, $test"
            }
            Write-Output "PASS: Structure $structure, $test"
        }
    }
}
```

The command above runs the application checks. Add the team’s full test command here when the oracle, differential test, and invariant checker are integrated.

## Select seeded hashing

Both structures use their fixed polynomial hash by default. Pass `--seed` after the structure option to select Ritik's shared `SeededHash`. Specify an unsigned 64-bit integer in decimal. For word mode, put `--words` after the seed.

```powershell
Get-Content tests/application_commands.in | .\app.exe --structure A --seed 42
Get-Content tests/application_commands.in | .\app.exe --structure B --seed 42
.\app.exe --structure B --seed 42 --words data/corpus.txt
```

The latest Structure A code defines the shared hash in `StructureA.h` and `StructureA.cpp`. It uses seeded position coefficients modulo 1,000,000,007. The application passes the same shared object to either table. Each table keeps its hash choice throughout the run, including during resizing. The shared function's theoretical collision bound should be explained by the team in Q5.

Structure B requires the constructor accepting `const SeededHash*` as its third argument. That constructor is included in this package. The older boolean constructor continues to select Kartikey's internal random-base mode. The application's `--seed` option uses the shared-object constructor. The application creates the hash before the table so the hash object outlives the table.

## Run attack mode

Ask the test owner to put the block-substitution operation file at `tests/attack_ops.txt`, or substitute its actual path in these commands. The file uses the same `I`, `G`, `D`, and `N` format as the command interface. Keys remain case-sensitive and punctuation is preserved in this mode.

```powershell
.\app.exe --structure A --seed 42 --attack tests/attack_ops.txt > attack_A.csv
.\app.exe --structure B --seed 42 --attack tests/attack_ops.txt > attack_B.csv
```

The runner reads the file into memory once, then replays it five times with fixed hashing and five times with seeded hashing. Each repetition creates two fresh tables. The seed stays the same across the five repetitions. It checks every query answer, the final key count, and the final presence/value of every key mentioned in the input. Invalid commands or a mismatch return a nonzero exit code.

Output is CSV with columns `run,structure,hash_mode,seed,replay_ms,final_keys,verification`. Five rows per mode are followed by the two mean-time rows. Timings include parsing the in-memory commands, table operations, and formatting query answers into memory. Disk input, table construction/destruction, validation, and terminal printing are outside the timer. The seeded hash generates its coefficients during hashing, which contributes to the recorded replay time.

The runner accepts any valid operation file. For a correctness control, it can also replay `tests/random_ops.txt` produced by Shubham's generator. Use the block-substitution file for the required collision experiment. Save the exact input file and seed with the experiment data. Adwit owns the report's benchmark script and figures.

## Compare fixed and seeded word counts

To verify that fixed and seeded hashing give the same counts on the full Gutenberg corpus, run:

```powershell
.\app.exe --structure A --seed 42 --compare-words data/corpus.txt
.\app.exe --structure B --seed 42 --compare-words data/corpus.txt
```

Each command starts with two empty tables and reads the same corpus for both runs. It checks every word and frequency and reports the number of distinct words if the outputs match. A mismatch returns a nonzero exit code.

## Benchmarks and figures

Pending team integration. Add the exact command to run the team benchmark and regenerate the figures once the measurement owner confirms it.

## Known limitations

- The application branch needs the two table implementations from the team repository to build.
- The test owner's block-substitution input file and the measurement owner's report figures are supplied through their branches. The application contains the replay runner and requires the actual operation file to run that experiment.
- The word counter recognizes ASCII letters. Accented letters and other non-ASCII characters are treated as separators.
