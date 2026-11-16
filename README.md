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

## Benchmarks and figures

Pending team integration. Add the exact command to run the team benchmark and regenerate the figures once the measurement owner confirms it.

## Known limitations

- The application branch needs the two table implementations from the team repository to build.
- The application does not yet have the required fixed-versus-seeded comparison mode.
- The word counter recognizes ASCII letters. Accented letters and other non-ASCII characters are treated as separators.
